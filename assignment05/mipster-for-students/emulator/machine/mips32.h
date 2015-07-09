// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#ifndef EMULATOR_MACHINE_MIPS32_H_
#define EMULATOR_MACHINE_MIPS32_H_

#include <err.h>
#include <stdint.h>
#include <string.h>

#include "emulator/machine/memory.h"
#include "emulator/machine/mips32_isa.h"

class Instrucion;
class OS;
class Process;

const int32_t kWordSizeInBytes = 4;
const int32_t kWordSize = 32;
const int32_t kDoubleWordSize = 2 * kWordSize;
const int32_t kNumMips32GeneralPurposeRegisters = 32;
const int32_t kGeneralPurposeRegisterLength = 32;
const int32_t kNumMips32FloatingPointRegisters = 32;

// Names from: http://msdn.microsoft.com/en-us/library/ms253512(VS.80).aspx
#define FOR_ALL_GENERAL_PURPOSE_REGISTERS(V)                                   \
  V(zero, 0)                                                                   \
  V(at, 1)                                                                     \
  V(v0, 2)                                                                     \
  V(v1, 3)                                                                     \
  V(a0, 4)                                                                     \
  V(a1, 5)                                                                     \
  V(a2, 6)                                                                     \
  V(a3, 7)                                                                     \
  V(t0, 8)                                                                     \
  V(t1, 9)                                                                     \
  V(t2, 10)                                                                    \
  V(t3, 11)                                                                    \
  V(t4, 12)                                                                    \
  V(t5, 13)                                                                    \
  V(t6, 14)                                                                    \
  V(t7, 15)                                                                    \
  V(s0, 16)                                                                    \
  V(s1, 17)                                                                    \
  V(s2, 18)                                                                    \
  V(s3, 19)                                                                    \
  V(s4, 20)                                                                    \
  V(s5, 21)                                                                    \
  V(s6, 22)                                                                    \
  V(s7, 23)                                                                    \
  V(t8, 24)                                                                    \
  V(t9, 25)                                                                    \
  V(k0, 26)                                                                    \
  V(k1, 27)  /* bump pointer register */                                       \
  V(gp, 28)                                                                    \
  V(sp, 29)                                                                    \
  V(fp, 30)                                                                    \
  V(ra, 31)

struct Registers {
  union {
    int32_t GPR[kNumMips32GeneralPurposeRegisters];

    struct {
#define DECLARE_GENERAL_PURPOSE_REGISTER(name, index)                          \
      int32_t name;

FOR_ALL_GENERAL_PURPOSE_REGISTERS(DECLARE_GENERAL_PURPOSE_REGISTER)
#undef DECLARE_GENERAL_PURPOSE_REGISTER
    };
  };

  // Floating point registers.
  float FPR[kNumMips32FloatingPointRegisters];

  union {
    int32_t raw;
    struct {
      int32_t RE : 1;
    };
  } status;

  int32_t LO;
  int32_t HI;
};


class Register {
 public:
  static const char* Name(int32_t gpr_index);

  enum BitfieldIndex  {
#define DECLARE_ENUM(name, idx) \
    name = 1LL << idx,

FOR_ALL_GENERAL_PURPOSE_REGISTERS(DECLARE_ENUM)
#undef DECLARE_ENUM
  };

  static inline int32_t BitfieldIndexFor(int32_t register_idx) {
    return 1L << register_idx;
  }
};


struct MachineState {};


struct Mips32State : MachineState {
  Registers registers;
  Memory* physical_memory;
  uintptr_t pc;
};


class Machine {
 public:
  virtual void Run() = 0;
};


class Mips32Machine : public Machine {
 public:
  static Mips32Machine& Create(intptr_t physical_memory_sz);
  static Mips32Machine& Get();

  void Run();
  void Recover(int32_t skip_instructions);
  Memory *GetMemory() { return state_->physical_memory; }

 private:
  explicit Mips32Machine(intptr_t physical_memory_sz);
  uint32_t FetchInstruction();

  inline Mips32State& MachineState() { return *state_; }



  Mips32State* state_;
  bool started_;

  static Mips32Machine* machine_;

  friend class Loader;
  friend class OS;
  friend class Process;
  friend class Instruction;
  friend class InstructionDecoder;
};

#endif  // EMULATOR_MACHINE_MIPS32_H_
