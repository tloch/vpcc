// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#ifndef EMULATOR_OS_PROCESS_H_
#define EMULATOR_OS_PROCESS_H_

#include <stdint.h>

#include <list>
#include <string>

#include "machine/mips32.h"
#include "machine/memory.h"

// one page above the stack is reserved for argc and argvs
const uint32_t kArgumentsSpace = 0x1000;

// An abstraction for a regular process control block (PCB).
class Process {
 public:
  Process(int32_t stack_addr, int32_t priority, std::string raw_argv);

  inline int32_t stack_addr() { return stack_addr_; }

  inline int32_t priority() { return priority_; }

  void SaveContext(const Mips32State& state);
  void RestoreContext(Mips32State* state);

 private:
  int32_t stack_addr_;
  int32_t priority_;

  // We do not save the context on the stack, but rather keep it in here.
  Registers saved_registers_;
  int32_t saved_pc_;
  char* saved_memory_;
};

#endif  // EMULATOR_OS_PROCESS_H_
