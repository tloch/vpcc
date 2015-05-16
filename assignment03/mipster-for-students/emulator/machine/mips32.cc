// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#include "machine/mips32.h"

#include <gflags/gflags.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "emulator/config.h"
#include "emulator/os/os.h"
#include "emulator/utils.h"


const char* Register::Name(int32_t gpr_index) {
  switch (gpr_index) {
#define RETURN_REGISTER_NAME(name, index)                                      \
    case index:                                                                \
      return TOSTRING(name);

FOR_ALL_GENERAL_PURPOSE_REGISTERS(RETURN_REGISTER_NAME)
#undef RETURN_REGISTER_NAME
  }
  return "";
}


Mips32Machine* Mips32Machine::machine_;


Mips32Machine& Mips32Machine::Create(intptr_t physical_memory_sz) {
  machine_ = new Mips32Machine(physical_memory_sz);
  return *machine_;
}


Mips32Machine& Mips32Machine::Get() {
  return *machine_;
}


Mips32Machine::Mips32Machine(intptr_t physical_memory_sz)
    : state_(new Mips32State()),
      started_(false) {

  state_->physical_memory = new Memory(physical_memory_sz);
  memset(&state_->registers, 0, sizeof(Registers));
  state_->pc = 0;

  // Lets add a JumpRegister Instruction at 0x0 so that we can start the machine
  // on this adress itself.
  // We use register zero which always holds zero, effectively jumping back to
  // address 0x0.
  RType jr_instruction = { 0 };
  jr_instruction.funct = 0x8;
  state_->physical_memory->Write(0, jr_instruction);
}


void Mips32Machine::Run() {
  OS::Get().Init();

  InstructionDecoder decoder;
  uint32_t raw = 0;
  Instruction* instruction = NULL;
  int64_t cycle_cnt = 0;
  while (true) {
    raw = FetchInstruction();
    instruction = decoder.Decode(raw);
    instruction->PreparePotentialDelaySlot();
    instruction->Execute();
    OS::Get().TimerInterrupt();
    cycle_cnt++;
  }
}


// Only needed for -torture_mode.
void Mips32Machine::Recover(int32_t skip_instructions) {
  InstructionDecoder decoder;
  uint32_t raw = 0;
  Instruction* instruction = NULL;
  while (skip_instructions-- > 0) {
    raw = FetchInstruction();
    state_->pc += sizeof(int32_t);
  }
  int64_t cycle_cnt = 0;
  while (true) {
    raw = FetchInstruction();
    instruction = decoder.Decode(raw);
    instruction->PreparePotentialDelaySlot();
    instruction->Execute();
    OS::Get().TimerInterrupt();
    cycle_cnt++;
  }
}


uint32_t Mips32Machine::FetchInstruction() {
  uint32_t instruction = 0;
  state_->physical_memory->Read(state_->pc, &instruction);
  return instruction;
}
