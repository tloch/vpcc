// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#include "machine/mips32_isa.h"

#include <assert.h>
#include <gflags/gflags.h>
#include <stdarg.h>
#include <stdint.h>

#include <limits>

#include "emulator/machine/mips32.h"
#include "emulator/os/os.h"
#include "emulator/utils.h"

DEFINE_bool(disassemble,            false,
            "Disassemble while executing.");
DEFINE_bool(debug_registers,        false,
            "Print register info after executing an instruction.");
DEFINE_bool(debug_delay_slot,       false,
            "Print delay slot information.");
DEFINE_bool(debug_register_writes,  false,
            "Print register write information.");

#define ISAM (BLACK "[I]")

static inline int32_t ZeroExtendFrom16(int16_t value) {
  int32_t r = 0x0000FFFF;
  return r &= value;
}


static inline int32_t ZeroExtendFrom8(int8_t value) {
  int32_t r = 0x000000FF;
  return r &= value;
}


static inline bool InInt32Range(int64_t value) {
  if ((value > std::numeric_limits<int32_t>::max()) ||
      (value < std::numeric_limits<int32_t>::min())) {
    return false;
  }
  return true;
}


// For instruction decoding we ask each instruction whether the current raw
// value matches its characteristics, for example, if op code and funct id are
// matching.
//
// Note: This is not a fast approach, since we repeatedly ask instructions that
// we know cannot be a candidate. However, it's easy to generalize decoding this
// way.
Instruction* InstructionDecoder::Decode(uint32_t raw) {
#define CREATE_AND_RETURN_INSTRUCTION(U1, U2, U3, NAME, U4)                    \
  if (NAME::Is##NAME(raw)) { return new NAME(raw); }

  FOR_ALL_INSTRUCTIONS(CREATE_AND_RETURN_INSTRUCTION)
#undef CREATE_AND_RETURN_INSTRUCTION

  // Emit some debugging code to identify unknown instructions.
  const uintptr_t op_code = raw >> 26;
  printf("\n%p  unknown instruction, op code: %" PRIuPTR  "\n\n",
         reinterpret_cast<void*>(Mips32Machine::Get().MachineState().pc),
         op_code);
  if (op_code == 0) {
    RType& r_raw = reinterpret_cast<RType&>(raw);
    printf("            funct: 0x%x\n", r_raw.funct);
  }
  errx(EXIT_FAILURE, "stopping execution");
}


Mips32State& Instruction::MachineState() {
  return Mips32Machine::Get().MachineState();
}


// Increments the PC by a single word if we are not in a delay slot instruction.
void Instruction::NextInstruction() {
  if (!is_delay_slot()) {
    MachineState().pc += kInstructionSize;
  }
}


int32_t Instruction::ReverseEndian() {
  int32_t value = MachineState().registers.status.RE;
  // We only support LE.
  assert(value == 0);
  return value;
}


int32_t Instruction::BigEndianCPU() {
  int32_t value = ReverseEndian() ^ BigEndianMem();
  // We only support LE.
  assert(value == 0);
  return value;
}


// Translate virtual addresses into physical ones using this function.
int32_t Instruction::AddressTranslation(int32_t virtual_address) {
  // For now vaddr == paddr. Later on use the operating system's page table.
  return virtual_address;
}


// Helper for printing disassembly code. The helper automatically checks for the
// debug flag and prepends the current PC.
void Instruction::Disassemble(const char* format, ...) {
  if (FLAGS_disassemble) {
    const int kLineLength = 256;
    const char* kDelaySlotInfo = " [delay slot]";
    int rest = kLineLength;
    char line[kLineLength];
    int written = snprintf(
        line,
        rest,
        MAGENTA "[D]\t%p  ",
        reinterpret_cast<void*>(MachineState().pc));
    assert(written >= 0);
    rest -= written;
    assert(rest > 0);
    va_list argptr;
    va_start(argptr, format);
    written = vsnprintf(&line[kLineLength - rest], rest, format, argptr);
    rest -= written;
    va_end(argptr);
    if (is_delay_slot()) {
      written = snprintf(&line[kLineLength - rest], rest, "%s", kDelaySlotInfo);
      rest -= written;
    }
    fprintf(stdout, "%s" RESET "\n", line);
  }
}


// Assumption: PC is still on the actual instruction.
void Instruction::PreparePotentialDelaySlot() {
  if (needs_delay_slot_) {
    if (FLAGS_debug_delay_slot) {
      printf("preparing delay slot\n");
    }
    MachineState().physical_memory->Read(
        MachineState().pc + kInstructionSize, &delay_slot_raw_);
  }
}


// From the manual:
//
// A taken branch assigns the target address to the PC during the instruction
// time of the instruction in the branch delay slot.
void Instruction::ExecuteDelaySlot() {
  if (!needs_delay_slot_) {
    errx(EXIT_FAILURE, "instruction should not have called ExecuteDelaySlot() "
                       "since it does not have a delay slot");
  }
  InstructionDecoder decoder;
  Instruction* instruction = decoder.Decode(delay_slot_raw_);
  instruction->set_is_delay_slot();
  instruction->Execute();
}


void Instruction::DebugRegisters(int64_t register_bitfield,
                                 bool include_special_registers) {
  if (FLAGS_debug_registers) {
    if (include_special_registers) {
      register_bitfield |= Register::BitfieldIndex::sp;
      register_bitfield |= Register::BitfieldIndex::ra;
      register_bitfield |= Register::BitfieldIndex::gp;
    }
    for (int i = 0; i < kNumMips32GeneralPurposeRegisters; ++i) {
      if (1UL << i & register_bitfield) {
        printf(CYAN "[R]\t%s (%d): %08x" RESET "\n" ,
               Register::Name(i), i, MachineState().registers.GPR[i]);
      }
    }
  }
}


void Instruction::WriteRegister(int32_t reg, int32_t value) {
  if (reg == 0) {
    if (FLAGS_debug_register_writes) {
      printf("ignoring write to register $0\n");
    }
    return;
  }
  MachineState().registers.GPR[reg] = value;
}


void NoOperationInstruction::Execute() {
  Disassemble("nop");
  NextInstruction();
}


void LoadUpperImmediateInstruction::Execute() {
  // OP-Code: 001111
  // GPR[rt] <- immediate || 0^16
  Disassemble("lui %s,0x%x",
              Register::Name(InstructionData().rt),
              ZeroExtendFrom16(InstructionData().immediate));
  assert(InstructionData().rs == 0x0);
  int32_t tmp = InstructionData().immediate << 16;
  WriteRegister(InstructionData().rt, tmp);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rt));
}


void OrImmediateInstruction::Execute() {
  // TODO(students): implement
//  ErrorOut(EXIT_FAILURE, ISAM, "ori not yet implemented");
  Disassemble("ori %s,%s,0x%x",
              Register::Name(InstructionData().rt),
              Register::Name(InstructionData().rs),
              ZeroExtendFrom16(InstructionData().immediate));
  int32_t temp =
      MachineState().registers.GPR[InstructionData().rs] |
      ZeroExtendFrom16(InstructionData().immediate);
  WriteRegister(InstructionData().rt, temp);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rt) |
      Register::BitfieldIndexFor(InstructionData().rs));
}


void AddUnsignedWordInstruction::Execute() {
  Disassemble("addu %s,%s,%s",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt));
  int32_t temp = MachineState().registers.GPR[InstructionData().rs] +
                 MachineState().registers.GPR[InstructionData().rt];
  WriteRegister(InstructionData().rd, temp);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rd) |
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void AddImmediateUnsignedWordInstruction::Execute() {
  Disassemble("addiu %s,%s,%d",
              Register::Name(InstructionData().rt),
              Register::Name(InstructionData().rs),
              InstructionData().immediate);
  int32_t temp = MachineState().registers.GPR[InstructionData().rs] +
  InstructionData().immediate;
  WriteRegister(InstructionData().rt, temp);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rt) |
      Register::BitfieldIndexFor(InstructionData().rs));
}


void AddImmediateWordInstruction::Execute() {
  Disassemble("addi %s,%s,%d",
              Register::Name(InstructionData().rt),
              Register::Name(InstructionData().rs),
              InstructionData().immediate);
  int64_t temp =
      static_cast<int64_t>(MachineState().registers.GPR[InstructionData().rs]) +
      InstructionData().immediate;
  if (!InInt32Range(temp)) {
    OS::Get().ExceptionHandler(kInterOverflow);
    UNREACHABLE();
  }
  WriteRegister(InstructionData().rt, static_cast<int32_t>(temp));
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void BranchOnNotEqualInstruction::Execute() {
  // if rs != rt then branch
  Disassemble("bne %s,%s,0x%x",
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt),
              InstructionData().immediate);
  // I: target_offset <- sign_extend(offset || 0^2)
  // condition <- (GPR[rs] != GPR[rt])
  int32_t target_offset = InstructionData().immediate << 2;
  assert((target_offset & 0x3) == 0);  // Double check concatenation.
  bool condition = MachineState().registers.GPR[InstructionData().rs] !=
                   MachineState().registers.GPR[InstructionData().rt];
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
  if (condition) {
    MachineState().pc += target_offset;
    ExecuteDelaySlot();
  }
}


void AddWordInstruction::Execute() {
  Disassemble("add %s,%s,%s",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt));
  int64_t temp =
      static_cast<int64_t>(MachineState().registers.GPR[InstructionData().rs]) +
      static_cast<int64_t>(MachineState().registers.GPR[InstructionData().rt]);
  if (!InInt32Range(temp)) {
    OS::Get().ExceptionHandler(kInterOverflow);
    UNREACHABLE();
  }
  WriteRegister(InstructionData().rd, static_cast<int32_t>(temp));
  NextInstruction();
}


void SubtractWordInstruction::Execute() {
  Disassemble("sub %s,%s,%s",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt));
  int64_t temp =
      static_cast<int64_t>(MachineState().registers.GPR[InstructionData().rs]) -
      static_cast<int64_t>(MachineState().registers.GPR[InstructionData().rt]);
  if (!InInt32Range(temp)) {
    OS::Get().ExceptionHandler(kInterOverflow);
    UNREACHABLE();
  }
  WriteRegister(InstructionData().rd, temp);
  NextInstruction();
}


void SubtractUnsignedWordInstruction::Execute() {
  Disassemble("subu %s,%s,%s",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt));
  // temp <- GPR[rs] - GPR[rt]
  int32_t temp = MachineState().registers.GPR[InstructionData().rs] -
                 MachineState().registers.GPR[InstructionData().rt];
  // GPR[rd] <- temp
  WriteRegister(InstructionData().rd, temp);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rd) |
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void MultiplyWordInstruction::Execute() {
  Disassemble("mult %s,%s",
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt));
  int64_t result =
      static_cast<int64_t>(MachineState().registers.GPR[InstructionData().rs]) *
      static_cast<int64_t>(MachineState().registers.GPR[InstructionData().rt]);
  MachineState().registers.LO = result & 0xFFFFFFFF;
  MachineState().registers.HI = (result  >> 32) & 0xFFFFFFFF;
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}

void MultiplyToGPRInstruction::Execute() {
  Disassemble("mul %s,%s,%s",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt));
  int64_t temp =
      static_cast<int64_t>(MachineState().registers.GPR[InstructionData().rs]) *
      static_cast<int64_t>(MachineState().registers.GPR[InstructionData().rt]);
/*
  if (!InInt32Range(temp)) {
    OS::Get().ExceptionHandler(kInterOverflow);
    UNREACHABLE();
  }
*/
  WriteRegister(InstructionData().rd, static_cast<int32_t>(temp));
  NextInstruction();
}


void MoveFromLORegisterInstruction::Execute() {
  Disassemble("mflo %s", Register::Name(InstructionData().rd));
  WriteRegister(InstructionData().rd, MachineState().registers.LO);
  NextInstruction();
  DebugRegisters(Register::BitfieldIndexFor(InstructionData().rd));
}


void MoveFromHIRegisterInstruction::Execute() {
  Disassemble("mfhi %s", Register::Name(InstructionData().rd));
  WriteRegister(InstructionData().rd, MachineState().registers.HI);
  NextInstruction();
  DebugRegisters(Register::BitfieldIndexFor(InstructionData().rd));
}


void MultiplyUnsignedWordInstruction::Execute() {
  Disassemble("multu %s,%s",
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt));
  uint64_t rs_unsigned = static_cast<uint64_t>(
      MachineState().registers.GPR[InstructionData().rs]) & 0xFFFFFFFFU;
  uint64_t rt_unsigned = static_cast<uint64_t>(
      MachineState().registers.GPR[InstructionData().rt]) & 0xFFFFFFFFU;
  uint64_t prod = rs_unsigned * rt_unsigned;
  MachineState().registers.LO = prod & 0xFFFFFFFFU;
  MachineState().registers.HI = (prod  >> 32U) & 0xFFFFFFFFU;
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void DivideWordInstruction::Execute() {
  Disassemble("div %s,%s",
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt));
  int32_t rs = MachineState().registers.GPR[InstructionData().rs];
  int32_t rt = MachineState().registers.GPR[InstructionData().rt];
  MachineState().registers.LO = rs / rt;
  MachineState().registers.HI = rs % rt;
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void DivideUnsignedWordInstruction::Execute() {
  Disassemble("divu %s,%s",
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt));
  uint32_t rs_unsigned = static_cast<uint32_t>(
      MachineState().registers.GPR[InstructionData().rs]);
  uint32_t rt_unsigned = static_cast<uint32_t>(
      MachineState().registers.GPR[InstructionData().rt]);
  MachineState().registers.LO = rs_unsigned / rt_unsigned;
  MachineState().registers.HI = rs_unsigned % rt_unsigned;
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void ExclusiveORInstruction::Execute() {
  Disassemble("xor %s,%s,%s",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt));
  int32_t temp =
      MachineState().registers.GPR[InstructionData().rs] ^
      MachineState().registers.GPR[InstructionData().rt];
  WriteRegister(InstructionData().rd, temp);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rd) |
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void MoveToHIRegisterInstruction::Execute() {
  Disassemble("mthi %s", Register::Name(InstructionData().rs));
  MachineState().registers.HI =
      MachineState().registers.GPR[InstructionData().rs];
  NextInstruction();
  DebugRegisters(Register::BitfieldIndexFor(InstructionData().rs));
}


void MoveToLORegisterInstruction::Execute() {
  Disassemble("mtlo %s", Register::Name(InstructionData().rs));
  MachineState().registers.LO =
      MachineState().registers.GPR[InstructionData().rs];
  NextInstruction();
  DebugRegisters(Register::BitfieldIndexFor(InstructionData().rs));
}


void BreakInstruction::Execute() {
  // Break as specified in the manual differs from the gcc
  // implementation. The code is encoded starting in the range of bit 16-25.
  int32_t code = raw_ >> 16;  // Works as op code == 0.
  Disassemble("break %d", code);
  errx(code, "exiting");
}


void JumpInstruction::Execute() {
  // TODO(students): implement
//  ErrorOut(EXIT_FAILURE, ISAM, "j not yet implemented");
  // PC <- PC_GPRLEN..28 || instr_index || 0^2
  uint32_t new_pc = MachineState().pc & 0xF0000000;
  new_pc |= InstructionData().address << 2;
  Disassemble("j %p", reinterpret_cast<void*>(new_pc));
  MachineState().pc = new_pc;
  DebugRegisters(Register::BitfieldIndex::ra);
  ExecuteDelaySlot();
}


void JumpAndLinkInstruction::Execute() {
  // GPR[31] <- PC + 8
  MachineState().registers.GPR[31] = MachineState().pc + 8;
  // PC <- PC_GPRLEN..28 || instr_index || 0^2
  uint32_t new_pc = MachineState().pc & 0xF0000000;
  new_pc |= InstructionData().address << 2;
  Disassemble("jal %p", reinterpret_cast<void*>(new_pc));
  MachineState().pc = new_pc;
  DebugRegisters(Register::BitfieldIndex::ra);
  ExecuteDelaySlot();
}


void JumpRegisterInstruction::Execute() {
  Disassemble("jr %s", Register::Name(InstructionData().rs));
  int32_t reg = InstructionData().rs;
  uint32_t jump_to = MachineState().registers.GPR[reg];
  MachineState().pc = jump_to;
  DebugRegisters(Register::BitfieldIndexFor(InstructionData().rs));
  ExecuteDelaySlot();
}


void JumpAndLinkRegisterInstruction::Execute() {
  assert(InstructionData().shamt == 0);  // Only hint == 0 is allowed.
  Disassemble("jalr %s,%s",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rs));
  WriteRegister(InstructionData().rd, MachineState().pc + 8);
  MachineState().pc = MachineState().registers.GPR[InstructionData().rs];
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rd) |
      Register::BitfieldIndexFor(InstructionData().rs));
  ExecuteDelaySlot();
}


void BranchOnEqualInstruction::Execute() {
  Disassemble("beq %s,%s,%d",
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt),
              InstructionData().immediate);
  // I: target_offset <- sign_extend(offset || 0^2)
  // condition <- (GPR[rs] != GPR[rt])
  int32_t target_offset = InstructionData().immediate << 2;
  assert((target_offset & 0x3) == 0);  // Double check concatenation.
  bool condition = MachineState().registers.GPR[InstructionData().rs] ==
                   MachineState().registers.GPR[InstructionData().rt];
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
  if (condition) {
    MachineState().pc += target_offset;
    ExecuteDelaySlot();
  }
}


void BranchOnLessThanOrEqualToZeroInstruction::Execute() {
  Disassemble("blez %s,%d",
              Register::Name(InstructionData().rs),
              InstructionData().immediate);
  int32_t target_offset = InstructionData().immediate << 2;
  assert((target_offset & 0x3) == 0);  // Double check concatenation.
  bool condition = MachineState().registers.GPR[InstructionData().rs] <= 0;
  NextInstruction();
  DebugRegisters(Register::BitfieldIndexFor(InstructionData().rs));
  if (condition) {
    MachineState().pc += target_offset;
    ExecuteDelaySlot();
  }
}


void BranchOnLessThanZeroInstruction::Execute() {
  Disassemble("bltz %s,%d",
              Register::Name(InstructionData().rs),
              InstructionData().immediate);
  int32_t target_offset = InstructionData().immediate << 2;
  assert((target_offset & 0x3) == 0);  // Double check concatenation.
  bool condition = MachineState().registers.GPR[InstructionData().rs] < 0;
  NextInstruction();
  DebugRegisters(Register::BitfieldIndexFor(InstructionData().rs));
  if (condition) {
    MachineState().pc += target_offset;
    ExecuteDelaySlot();
  }
}


void BranchOnLessThanZeroAndLinkInstruction::Execute() {
  Disassemble("bltzal %s,%d",
              Register::Name(InstructionData().rs),
              InstructionData().immediate);
  int32_t target_offset = InstructionData().immediate << 2;
  assert((target_offset & 0x3) == 0);  // Double check concatenation.
  bool condition = MachineState().registers.GPR[InstructionData().rs] < 0;
  WriteRegister(31, MachineState().pc + 8);
  NextInstruction();
  DebugRegisters(Register::BitfieldIndexFor(InstructionData().rs));
  if (condition) {
    MachineState().pc += target_offset;
    ExecuteDelaySlot();
  }
}


void BranchOnGreaterThanOrEqualToZeroInstruction::Execute() {
  Disassemble("bgez %s,%d",
              Register::Name(InstructionData().rs),
              InstructionData().immediate);
  int32_t target_offset = InstructionData().immediate << 2;
  assert((target_offset & 0x3) == 0);  // Double check concatenation.
  bool condition = MachineState().registers.GPR[InstructionData().rs] >= 0;
  NextInstruction();
  DebugRegisters(Register::BitfieldIndexFor(InstructionData().rs));
  if (condition) {
    MachineState().pc += target_offset;
    ExecuteDelaySlot();
  }
}


void BranchOnGreaterThanZeroInstruction::Execute() {
  assert(InstructionData().rt == 0);
  Disassemble("bgtz %s,%d",
              Register::Name(InstructionData().rs),
              InstructionData().immediate);
  int32_t target_offset = InstructionData().immediate << 2;
  bool condition = MachineState().registers.GPR[InstructionData().rs] > 0;
  NextInstruction();
  DebugRegisters(Register::BitfieldIndexFor(InstructionData().rs));
  if (condition) {
    MachineState().pc += target_offset;
    ExecuteDelaySlot();
  }
}


void BranchOnGreaterThanOrEqualToZeroAndLinkInstruction::Execute() {
  Disassemble("bgezal %s,%d",
              Register::Name(InstructionData().rs),
              InstructionData().immediate);
  int32_t target_offset = InstructionData().immediate << 2;
  bool condition = MachineState().registers.GPR[InstructionData().rs] >= 0;
  WriteRegister(31, MachineState().pc + 8);
  NextInstruction();
  DebugRegisters(Register::BitfieldIndexFor(InstructionData().rs));
  if (condition) {
    MachineState().pc += target_offset;
    ExecuteDelaySlot();
  }
}


void SystemcallInstruction::Execute() {
  Disassemble("syscall");
  OS::Get().Syscall();
  NextInstruction();
}


void LoadByteInstruction::Execute() {
  Disassemble("lb %s,%d(%s)",
              Register::Name(InstructionData().rt),
              InstructionData().immediate,
              Register::Name(InstructionData().rs));
  // vAddr <- sign_extend(offset) + GPR[base]
  int32_t vaddr = InstructionData().immediate +
                  MachineState().registers.GPR[InstructionData().rs];
  // (pAddr, CCA) <- AddressTranslation (vAddr, DATA, LOAD)
  int32_t paddr = AddressTranslation(vaddr);
  int8_t byte;
  MachineState().physical_memory->Read(paddr, &byte);
  WriteRegister(InstructionData().rt, byte);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void LoadByteUnsignedInstruction::Execute() {
  Disassemble("lbu %s,%d(%s)",
              Register::Name(InstructionData().rt),
              InstructionData().immediate,
              Register::Name(InstructionData().rs));
  // vAddr <- sign_extend(offset) + GPR[base]
  int32_t vaddr = InstructionData().immediate +
                  MachineState().registers.GPR[InstructionData().rs];
  // (pAddr, CCA) <- AddressTranslation (vAddr, DATA, LOAD)
  int32_t paddr = AddressTranslation(vaddr);
  int8_t byte;
  MachineState().physical_memory->Read(paddr, &byte);
  WriteRegister(InstructionData().rt, ZeroExtendFrom8(byte));
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void LoadHalfwordInstruction::Execute() {
  Disassemble("lh %s,%d(%s)",
              Register::Name(InstructionData().rt),
              InstructionData().immediate,
              Register::Name(InstructionData().rs));
  // vAddr <- sign_extend(offset) + GPR[base]
  int32_t vaddr = InstructionData().immediate +
                  MachineState().registers.GPR[InstructionData().rs];
  // if vAddr_{0} != 0 then SignalException(AddressError)
  if ((vaddr & 0x1) != 0) {
    OS::Get().ExceptionHandler(ExceptionType::kAddressError);
    UNREACHABLE();
  }
  // (pAddr, CCA) <- AddressTranslation (vAddr, DATA, LOAD)
  int32_t paddr = AddressTranslation(vaddr);
  int16_t halfword;
  MachineState().physical_memory->Read(paddr, &halfword);
  WriteRegister(InstructionData().rt, halfword);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rt) |
      Register::BitfieldIndexFor(InstructionData().rs));
}


void LoadHalfwordUnsignedInstruction::Execute() {
  Disassemble("lhu %s,%d(%s)",
              Register::Name(InstructionData().rt),
              InstructionData().immediate,
              Register::Name(InstructionData().rs));
  // vAddr <- sign_extend(offset) + GPR[base]
  int32_t vaddr = InstructionData().immediate +
                  MachineState().registers.GPR[InstructionData().rs];
  // if vAddr_{0} != 0 then SignalException(AddressError)
  if ((vaddr & 0x1) != 0) {
    OS::Get().ExceptionHandler(ExceptionType::kAddressError);
    UNREACHABLE();
  }
  // (pAddr, CCA) <- AddressTranslation (vAddr, DATA, LOAD)
  int32_t paddr = AddressTranslation(vaddr);
  int16_t halfword;
  MachineState().physical_memory->Read(paddr, &halfword);
  WriteRegister(InstructionData().rt, ZeroExtendFrom16(halfword));
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rt) |
      Register::BitfieldIndexFor(InstructionData().rs));
}


void LoadWordInstruction::Execute() {
  // OP-Code 100011
  // vAddr <- sign_extend(offset) + GPR[base]
  Disassemble("lw %s,%d(%s)",
              Register::Name(InstructionData().rt),
              InstructionData().immediate,
              Register::Name(InstructionData().rs));
  int32_t vaddr = MachineState().registers.GPR[InstructionData().rs] +
  InstructionData().immediate;
  if ((vaddr & 0x3) != 0) {
    OS::Get().ExceptionHandler(ExceptionType::kAddressError);
    UNREACHABLE();
  }
  // (pAddr, CCA) <- AddressTranslation(vAddr, DATA, LOAD)
  // We can ignore IorD, LorS and CCA as we dont have caches and only fake the
  // MMU.
  int32_t paddr = AddressTranslation(vaddr);
  int32_t memword;
  MachineState().physical_memory->Read(paddr, &memword);
  WriteRegister(InstructionData().rt, memword);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rt) |
      Register::BitfieldIndexFor(InstructionData().rs));
}


void StoreByteInstruction::Execute() {
  Disassemble("sb %s,%d(%s)",
              Register::Name(InstructionData().rt),
              InstructionData().immediate,
              Register::Name(InstructionData().rs));
  int32_t vaddr = MachineState().registers.GPR[InstructionData().rs] +
                  InstructionData().immediate;
  int32_t paddr = AddressTranslation(vaddr);
  MachineState().physical_memory->Write(
      paddr,
      static_cast<int8_t>(MachineState().registers.GPR[InstructionData().rt]));
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rt) |
      Register::BitfieldIndexFor(InstructionData().rs));
}


void StoreHalfwordInstruction::Execute() {
  Disassemble("sh %s,%d(%s)",
              Register::Name(InstructionData().rt),
              InstructionData().immediate,
              Register::Name(InstructionData().rs));
  // vAddr <- sign_extend(offset) + GPR[base]
  int32_t vaddr = InstructionData().immediate +
                  MachineState().registers.GPR[InstructionData().rs];
  // if vAddr_{0} != 0 then SignalException(AddressError)
  if ((vaddr & 0x1) != 0) {
    OS::Get().ExceptionHandler(ExceptionType::kAddressError);
    UNREACHABLE();
  }
  // (pAddr, CCA) <- AddressTranslation (vAddr, DATA, LOAD)
  int32_t paddr = AddressTranslation(vaddr);
  MachineState().physical_memory->Write(
      paddr,
      static_cast<int16_t>(MachineState().registers.GPR[InstructionData().rt]));
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rt) |
      Register::BitfieldIndexFor(InstructionData().rs));
}


void StoreWordInstruction::Execute() {
  // vAddr <- sign_extend(offset) + GPR[base]
  int32_t value = static_cast<int16_t>(InstructionData().immediate);
  Disassemble("sw %s,%d(%s)",
              Register::Name(InstructionData().rt),
              value,
              Register::Name(InstructionData().rs));
  int32_t vaddr = value + MachineState().registers.GPR[InstructionData().rs];
  // if vAddr_1..0 != 0^2 then SignalException(AddressError)
  if ((vaddr & 0x3) != 0) {
    OS::Get().ExceptionHandler(ExceptionType::kAddressError);
    UNREACHABLE();
  }
  // (pAddr, CCA) <- AddressTranslation(vAddr, DATA, STORE)
  // We can ignore IorD, LorS and CCA as we dont have caches and only fake the
  // MMU.
  int32_t paddr = AddressTranslation(vaddr);
  // dataword <- GPR[rt]
  int32_t dataword = MachineState().registers.GPR[InstructionData().rt];
  // StoreMemory(CCA, WORD, dataword, pAddr, vAddr, DATA)
  MachineState().physical_memory->Write(paddr, dataword);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void AndImmediateInstruction::Execute() {
  // OP-Code: 001100
  // rt <- rs AND immediate
  // The 16-bit immediate is zero-extended to the left and combined
  // with the contents of GPR rs in a bitwise logical AND
  // operation. The result is placed into GPR rt.
  Disassemble("andi %s,%s,0x%x",
              Register::Name(InstructionData().rt),
              Register::Name(InstructionData().rs),
              ZeroExtendFrom16(InstructionData().immediate));
  int32_t temp =
      MachineState().registers.GPR[InstructionData().rs] &
      ZeroExtendFrom16(InstructionData().immediate);
  WriteRegister(InstructionData().rt, temp);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rt) |
      Register::BitfieldIndexFor(InstructionData().rs));
}


void AndInstruction::Execute() {
  Disassemble("and %s,%s,%s",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt));
  int32_t temp =
      MachineState().registers.GPR[InstructionData().rs] &
      MachineState().registers.GPR[InstructionData().rt];
  WriteRegister(InstructionData().rd, temp);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rd) |
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void ExclusiveOrImmediateInstruction::Execute() {
  Disassemble("xori %s,%s,%d",
              Register::Name(InstructionData().rt),
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().immediate));
  int32_t temp = ZeroExtendFrom16(InstructionData().immediate);
  WriteRegister(InstructionData().rt,
                MachineState().registers.GPR[InstructionData().rs] ^ temp);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void OrInstruction::Execute() {
  Disassemble("or %s,%s,%s",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt));
  int32_t temp =
      MachineState().registers.GPR[InstructionData().rs] |
      MachineState().registers.GPR[InstructionData().rt];
  WriteRegister(InstructionData().rd, temp);
  NextInstruction();
}


void NotOrInstruction::Execute() {
  Disassemble("nor %s,%s,%s",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt));
  WriteRegister(InstructionData().rd,
      ~(MachineState().registers.GPR[InstructionData().rs] |
        MachineState().registers.GPR[InstructionData().rt]));
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rd) |
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void ShiftWordLeftLogicalInstruction::Execute() {
  Disassemble("sll %s,%s,%d",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rt),
              InstructionData().shamt);
  int32_t s = InstructionData().shamt;
  int32_t temp = MachineState().registers.GPR[InstructionData().rt] << s;
  WriteRegister(InstructionData().rd, temp);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rd) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void ShiftWordRightLogicalInstruction::Execute() {
  Disassemble("srl %s,%s,%d",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rt),
              InstructionData().shamt);
  int32_t s = InstructionData().shamt;
  int32_t temp = static_cast<uint32_t>(
      MachineState().registers.GPR[InstructionData().rt]) >> s;
  WriteRegister(InstructionData().rd, temp);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rd) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void ShiftWordRightArithmeticInstruction::Execute() {
  Disassemble("sra %s,%s,%d",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rt),
              InstructionData().shamt);
  int32_t s = InstructionData().shamt;
  int32_t temp = MachineState().registers.GPR[InstructionData().rt] >> s;
  WriteRegister(InstructionData().rd, temp);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rd) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void ShiftWordLeftLogicalVariableInstruction::Execute() {
  Disassemble("sllv %s,%s,%s",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rt),
              Register::Name(InstructionData().rs));
  int32_t s = MachineState().registers.GPR[InstructionData().rs] & 0x1FU;
  int32_t temp = MachineState().registers.GPR[InstructionData().rt] << s;
  WriteRegister(InstructionData().rd, temp);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rd) |
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void ShiftWordRightLogicalVariableInstruction::Execute() {
  Disassemble("srlv %s,%s,%s",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rt),
              Register::Name(InstructionData().rs));
  int32_t s = MachineState().registers.GPR[InstructionData().rs] & 0x1FU;
  int32_t temp = static_cast<uint32_t>(
      MachineState().registers.GPR[InstructionData().rt]) >> s;
  WriteRegister(InstructionData().rd, temp);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rd) |
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void ShiftWordRightArithmeticVariableInstruction::Execute() {
  Disassemble("srav %s,%s,%s",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rt),
              Register::Name(InstructionData().rs));
  int32_t s = MachineState().registers.GPR[InstructionData().rs] & 0x1FU;
  int32_t temp = MachineState().registers.GPR[InstructionData().rt] >> s;
  WriteRegister(InstructionData().rd, temp);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rd) |
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void SetOnLessThanImmediateInstruction::Execute() {
  Disassemble("slti %s,%s,%d",
              Register::Name(InstructionData().rt),
              Register::Name(InstructionData().rs),
              InstructionData().immediate);
  if (MachineState().registers.GPR[InstructionData().rs] <
      InstructionData().immediate) {
    WriteRegister(InstructionData().rt, 1);
  } else {
    WriteRegister(InstructionData().rt, 0);
  }
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}

void SetOnLessThanImmediateUnsignedInstruction::Execute() {
  Disassemble("sltiu %s,%s,%d",
              Register::Name(InstructionData().rt),
              Register::Name(InstructionData().rs),
              InstructionData().immediate);
  uint64_t rs_unsigned = static_cast<uint64_t>(
      MachineState().registers.GPR[InstructionData().rs]) & 0xFFFFFFFFU;
  uint64_t immediate_unsigned = static_cast<uint64_t>(
      static_cast<int32_t>(InstructionData().immediate)) & 0xFFFFFFFFU;
  if (rs_unsigned < immediate_unsigned) {
    WriteRegister(InstructionData().rt, 1);
  } else {
    WriteRegister(InstructionData().rt, 0);
  }
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void SetOnLessThanInstruction::Execute() {
  // TODO(students): implement
//  ErrorOut(EXIT_FAILURE, ISAM, "slt not yet implemented");
  Disassemble("slt %s,%s,%s",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt));
/*  int64_t temp =
      static_cast<int64_t>(MachineState().registers.GPR[InstructionData().rs]) +
      static_cast<int64_t>(MachineState().registers.GPR[InstructionData().rt]);*/

  int64_t rs_signed = static_cast<int64_t>(
      MachineState().registers.GPR[InstructionData().rs]
  );
  int64_t rt_signed = static_cast<int64_t>(
      MachineState().registers.GPR[InstructionData().rt]
  );
  if(rs_signed < rt_signed) {
    WriteRegister(InstructionData().rd, 1);
  } else {
    WriteRegister(InstructionData().rd, 0);
  }
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rd) |
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));

}


void SetOnLessThanUnsignedInstruction::Execute() {
  Disassemble("sltu %s,%s,%s",
              Register::Name(InstructionData().rd),
              Register::Name(InstructionData().rs),
              Register::Name(InstructionData().rt));
  uint64_t rs_unsigned = static_cast<uint64_t>(
      MachineState().registers.GPR[InstructionData().rs]) & 0xFFFFFFFFU;
  uint64_t rt_unsigned = static_cast<uint64_t>(
      MachineState().registers.GPR[InstructionData().rt]) & 0xFFFFFFFFU;
  if (rs_unsigned < rt_unsigned) {
    WriteRegister(InstructionData().rd, 1);
  } else {
    WriteRegister(InstructionData().rd, 0);
  }
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rd) |
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void ReadHardwareRegisterInstruction::Execute() {
  Disassemble("rdhwr %s,%d",
              Register::Name(InstructionData().rt),
              InstructionData().rd);
  switch (InstructionData().rd) {
    case 0:
    case 1:
    case 2:
    case 3:
      errx(EXIT_FAILURE, "rdhwr for rd 0-3 not implemented");
      break;
    default:
      OS::Get().ExceptionHandler(kReservedInstruction);
  }
  UNREACHABLE();
}


void LoadWordRightInstruction::Execute() {
  Disassemble("lwr %s,%d(%s)",
              Register::Name(InstructionData().rt),
              InstructionData().immediate,
              Register::Name(InstructionData().rs));
  int32_t vaddr = InstructionData().immediate +
                  MachineState().registers.GPR[InstructionData().rs];
  int32_t paddr = AddressTranslation(vaddr);
  uint32_t memword = 0;
  MachineState().physical_memory->Read(paddr & 0xFFFFFFFC, &memword);
  int32_t left_most = paddr & 0x3;
  int32_t f = MachineState().registers.GPR[InstructionData().rt];
  char* src = reinterpret_cast<char*>(&memword);
  memset(reinterpret_cast<void*>(&f), 0, 4 - left_most);
  memcpy(reinterpret_cast<void*>(&f), &src[left_most], 4 - left_most);
  WriteRegister(InstructionData().rt, f);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void LoadWordLeftInstruction::Execute() {
  Disassemble("lwl %s,%d(%s)",
              Register::Name(InstructionData().rt),
              InstructionData().immediate,
              Register::Name(InstructionData().rs));
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
  int32_t vaddr = InstructionData().immediate +
                  MachineState().registers.GPR[InstructionData().rs];
  int32_t paddr = AddressTranslation(vaddr);
  uint32_t memword = 0;
  MachineState().physical_memory->Read(paddr & 0xFFFFFFFC, &memword);
  int32_t len = (paddr & 0x3) + 1;
  int32_t f = MachineState().registers.GPR[InstructionData().rt];
  char* s1 = reinterpret_cast<char*>(&f);
  memset(&s1[4-len], 0, len);
  memcpy(&s1[4-len], &memword, len);
  WriteRegister(InstructionData().rt, f);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}


void StoreWordRight::Execute() {
  Disassemble("swr %s,%d(%s)",
              Register::Name(InstructionData().rt),
              InstructionData().immediate,
              Register::Name(InstructionData().rs));
  int32_t vaddr = InstructionData().immediate +
                  MachineState().registers.GPR[InstructionData().rs];
  int32_t paddr = AddressTranslation(vaddr);
  uint32_t memword = 0;
  MachineState().physical_memory->Read(paddr & 0xFFFFFFFC, &memword);
  int32_t w = MachineState().registers.GPR[InstructionData().rt];
  int32_t left_most = paddr & 0x3;
  char* s1 = reinterpret_cast<char*>(&w);
  MachineState().physical_memory->WriteBuffer(paddr, s1, 4-left_most);
  NextInstruction();
  DebugRegisters(
      Register::BitfieldIndexFor(InstructionData().rs) |
      Register::BitfieldIndexFor(InstructionData().rt));
}
