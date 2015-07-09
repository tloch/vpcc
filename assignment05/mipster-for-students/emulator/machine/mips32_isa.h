// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#ifndef EMULATOR_MACHINE_MIPS32_ISA_H_
#define EMULATOR_MACHINE_MIPS32_ISA_H_

#include <stdio.h>
#include <stdint.h>

struct Mips32State;
class Mips32Machine;
class OS;

//
// Formats for MIPS32 LE.
//

struct RType {
  uint32_t funct     : 6;
  uint32_t shamt     : 5;
  uint32_t rd        : 5;
  uint32_t rt        : 5;
  uint32_t rs        : 5;
  uint32_t op_code   : 6;
};


struct IType {
  int16_t immediate : 16;
  uint32_t rt        : 5;
  uint32_t rs        : 5;
  uint32_t op_code   : 6;
};


struct JType {
  uint32_t address   : 26;
  uint32_t op_code   : 6;
};


const int32_t kInstructionSize = sizeof(int32_t);


class Instruction {
 public:
  virtual void Execute() = 0;
  virtual ~Instruction() {}

  void PreparePotentialDelaySlot();

  inline bool has_delay_slot() { return needs_delay_slot_; }
  inline bool is_delay_slot() { return is_delay_slot_; }

 protected:
  explicit Instruction(uint32_t raw, bool needs_delay_slot)
      : raw_(raw),
        needs_delay_slot_(needs_delay_slot) {
    is_delay_slot_ = false;
  }

  inline void set_is_delay_slot() { is_delay_slot_ = true; }

  // Create shortcuts into the machine for all necessary operations since only
  // Instruction is a friend class.

  Mips32State& MachineState();

  int32_t AddressTranslation(int32_t virtual_address);
  void NextInstruction();

  void Disassemble(const char* format, ...);
  void DebugRegisters(int64_t register_bitfield,
                      bool include_special_registers= true);
  void ExecuteDelaySlot();

  inline int32_t BigEndianMem() {
    // 0: Little endian
    // 1: Big endian
    return 0;
  }

  int32_t ReverseEndian();
  int32_t BigEndianCPU();

  void WriteRegister(int32_t reg, int32_t value);

  uint32_t raw_;

  bool is_delay_slot_;
  bool needs_delay_slot_;
  uint32_t delay_slot_raw_;
};


template<typename T>
class FixedFormatInstruction : public Instruction {
 protected:
  explicit FixedFormatInstruction(uint32_t raw, bool needs_delay_slot)
      : Instruction(raw, needs_delay_slot) {}

  inline T& InstructionData() { return reinterpret_cast<T&>(raw_); }
};


class InstructionDecoder {
 public:
  InstructionDecoder() {}
  Instruction* Decode(uint32_t raw);
};


//
// In order to avoid declaring classes for the same type of instructions we use
// preprocessor macros to declare those classes.
//
// For all instructions we specify:
//     (pattern, value, type, name, needs_delay_slot)
//
//   pattern: The pattern indicating significant bits.
//   value: The value identifying the instruction on the significant bits.
//   type: I, R, or J
//   name: An instruction name
//   needs_delay_slot: true or false indicating wether the instruction requires
//                     a delay slot.

#define FOR_ALL_I_TYPE_INSTRUCTIONS(V)                                         \
  V(0xFC000000, 0x24000000, I, AddImmediateUnsignedWordInstruction, false)     \
  V(0xFC000000, 0x20000000, I, AddImmediateWordInstruction, false)             \
  V(0xFC000000, 0x30000000, I, AndImmediateInstruction, false)                 \
  V(0xFC000000, 0x10000000, I, BranchOnEqualInstruction, true)                 \
  V(0xFC1F0000, 0x04110000, I,                                                 \
      BranchOnGreaterThanOrEqualToZeroAndLinkInstruction, true)                \
  V(0xFC1F0000, 0x04010000, I,                                                 \
      BranchOnGreaterThanOrEqualToZeroInstruction, true)                       \
  V(0xFC1F0000, 0x1C000000, I, BranchOnGreaterThanZeroInstruction, true)       \
  V(0xFC1F0000, 0x18000000, I, BranchOnLessThanOrEqualToZeroInstruction, true) \
  V(0xFC1F0000, 0x04100000, I, BranchOnLessThanZeroAndLinkInstruction, true)   \
  V(0xFC1F0000, 0x04000000, I, BranchOnLessThanZeroInstruction, true)          \
  V(0xFC000000, 0x14000000, I, BranchOnNotEqualInstruction, true)              \
  V(0xFC000000, 0x38000000, I, ExclusiveOrImmediateInstruction, false)         \
  V(0xFC000000, 0x80000000, I, LoadByteInstruction, false)                     \
  V(0xFC000000, 0x90000000, I, LoadByteUnsignedInstruction, false)             \
  V(0xFC000000, 0x84000000, I, LoadHalfwordInstruction, false)                 \
  V(0xFC000000, 0x94000000, I, LoadHalfwordUnsignedInstruction, false)         \
  V(0xFFE00000, 0x3C000000, I, LoadUpperImmediateInstruction, false)           \
  V(0xFC000000, 0x8C000000, I, LoadWordInstruction, false)                     \
  V(0xFC000000, 0x88000000, I, LoadWordLeftInstruction, false)                 \
  V(0xFC000000, 0x98000000, I, LoadWordRightInstruction, false)                \
  V(0xFC000000, 0x34000000, I, OrImmediateInstruction, false)                  \
  V(0xFC000000, 0x28000000, I, SetOnLessThanImmediateInstruction, false)       \
  V(0xFC000000, 0x2C000000, I,                                                 \
      SetOnLessThanImmediateUnsignedInstruction, false)                        \
  V(0xFC000000, 0xA0000000, I, StoreByteInstruction, false)                    \
  V(0xFC000000, 0xA4000000, I, StoreHalfwordInstruction, false)                \
  V(0xFC000000, 0xAC000000, I, StoreWordInstruction, false)                    \
  V(0xFC000000, 0xB8000000, I, StoreWordRight, false)

#define FOR_ALL_R_TYPE_INSTRUCTIONS(V)                                         \
  V(0xFC0007FF, 0x00000021, R, AddUnsignedWordInstruction, false)              \
  V(0xFC0007FF, 0x00000020, R, AddWordInstruction, false)                      \
  V(0xFC0007FF, 0x00000024, R, AndInstruction, false)                          \
  V(0xFC00003F, 0x0000000D, R, BreakInstruction, false)                        \
  V(0xFC00FFFF, 0x0000001B, R, DivideUnsignedWordInstruction, false)           \
  V(0xFC00FFFF, 0x0000001A, R, DivideWordInstruction, false)                   \
  V(0xFC0007FF, 0x00000026, R, ExclusiveORInstruction, false)                  \
  V(0xFC1F003F, 0x00000009, R, JumpAndLinkRegisterInstruction, true)           \
  V(0xFC1FF83F, 0x00000008, R, JumpRegisterInstruction, true)                  \
  V(0xFFFF07FF, 0x00000010, R, MoveFromHIRegisterInstruction, false)           \
  V(0xFFFF07FF, 0x00000012, R, MoveFromLORegisterInstruction, false)           \
  V(0xFC1FFFFF, 0x00000011, R, MoveToHIRegisterInstruction, false)             \
  V(0xFC1FFFFF, 0x00000013, R, MoveToLORegisterInstruction, false)             \
  V(0xFC00FFFF, 0x00000019, R, MultiplyUnsignedWordInstruction, false)         \
  V(0xFC00FFFF, 0x00000018, R, MultiplyWordInstruction, false)                 \
  V(0xFC0007FF, 0x70000002, R, MultiplyWordRegisterInstruction, false)         \
  V(0xFC00003F, 0x00000034, R, TrapIfEqualInstruction, false)                  \
  V(0xFFFFFFFF, 0x00000000, R, NoOperationInstruction, false)                  \
  V(0xFC0007FF, 0x00000027, R, NotOrInstruction, false)                        \
  V(0xFC0007FF, 0x00000025, R, OrInstruction, false)                           \
  V(0xFFE007FF, 0x7C00003B, R, ReadHardwareRegisterInstruction, false)         \
  V(0xFC0007FF, 0x0000002A, R, SetOnLessThanInstruction, false)                \
  V(0xFC0007FF, 0x0000002B, R, SetOnLessThanUnsignedInstruction, false)        \
  V(0xFFE0003F, 0x00000000, R, ShiftWordLeftLogicalInstruction, false)         \
  V(0xFC0007FF, 0x00000004, R, ShiftWordLeftLogicalVariableInstruction, false) \
  V(0xFFE0003F, 0x00000002, R, ShiftWordRightLogicalInstruction, false)        \
  V(0xFFE0003F, 0x00000003, R, ShiftWordRightArithmeticInstruction, false)     \
  V(0xFC0007FF, 0x00000007, R,                                                 \
      ShiftWordRightArithmeticVariableInstruction, false)                      \
  V(0xFC0007FF, 0x00000006, R, ShiftWordRightLogicalVariableInstruction, false)\
  V(0xFC0007FF, 0x00000023, R, SubtractUnsignedWordInstruction, false)         \
  V(0xFC0007FF, 0x00000022, R, SubtractWordInstruction, false)                 \
  V(0xFC00003F, 0x0000000C, R, SystemcallInstruction, false)

#define FOR_ALL_J_TYPE_INSTRUCTIONS(V)                                         \
  V(0xFC000000, 0x0C000000, J, JumpAndLinkInstruction, true)                   \
  V(0xFC000000, 0x08000000, J, JumpInstruction, true)

#define FOR_ALL_INSTRUCTIONS(V)                                                \
  FOR_ALL_J_TYPE_INSTRUCTIONS(V)                                               \
  FOR_ALL_R_TYPE_INSTRUCTIONS(V)                                               \
  FOR_ALL_I_TYPE_INSTRUCTIONS(V)

#define DECLARE_INSTRUCTION(                                                   \
    PATTERN_BITS, VALUE_BITS, TYPE, NAME, DELAY_SLOT)                          \
class NAME : public FixedFormatInstruction<TYPE##Type> {                       \
 public:                                                                       \
  static inline bool Is##NAME(uint32_t raw) {                                  \
    if ((raw  & PATTERN_BITS) == VALUE_BITS) {                                 \
      return true;                                                             \
    }                                                                          \
    return false;                                                              \
  }                                                                            \
                                                                               \
  explicit NAME(uint32_t raw) : FixedFormatInstruction(raw, DELAY_SLOT) {}     \
                                                                               \
  void Execute();                                                              \
};

FOR_ALL_INSTRUCTIONS(DECLARE_INSTRUCTION)
#undef DECLARE_INSTRUCTION

#endif  // EMULATOR_MACHINE_MIPS32_ISA_H_
