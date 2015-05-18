// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#include "emulator/os/process.h"

#include <string.h>
#include <stdlib.h>

#include <list>
#include <sstream>
#include <string>

#include "emulator/machine/mips32.h"


static void split(
    const std::string &s, char delim, std::list<std::string>* elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems->push_back(item);
  }
}


Process::Process(int32_t stack_addr, int32_t priority, std::string raw_argv)
    : stack_addr_(stack_addr),
      priority_(priority) {
  Mips32Machine& m = Mips32Machine::Get();

  // O32 compatible binaries assume argc and argv upwards on the stack. We
  //
  // assume that we have the last page (4k) before SP to use for arguments.
  //
  // The final stack layout will look like:
  //
  // 4096(SP)         NULL
  //                  argv[x]
  // ...
  // num_arg*4+4(SP)  NULL
  // 12(SP)           argv[..]
  // 8(SP)            argv[1]
  // 4(SP)            argv[0]
  // 0(SP)            argc
  //
  // Note that we do not prepare the possibly third argument of main, envptr.

  std::list<std::string> parts;
  split(raw_argv, ' ', &parts);

  // Write argc.
  m.MachineState().physical_memory->Write(stack_addr_, parts.size());

  int32_t end = stack_addr_ + kArgumentsSpace;
  int32_t argv_base = stack_addr_ + kWordSizeInBytes;
  for (std::list<std::string>::const_iterator it = parts.begin();
       it != parts.end();
       ++it) {
    const int32_t arg_len = (*it).length()+ 1;
    const int32_t arg_start = end - arg_len;
    if (arg_start <= argv_base) {
      errx(EXIT_FAILURE, "unable to push argument onto stack... overflow");
    }
    char* arg_str = new char[arg_len];
    strncpy(arg_str, (*it).c_str(), arg_len - 1);
    arg_str[arg_len - 1] = 0;

    // Write argv[x].
    m.MachineState().physical_memory->WriteBuffer(arg_start, arg_str, arg_len);
    // Write &argv[x].
    m.MachineState().physical_memory->Write(argv_base, arg_start);

    end = arg_start;
    argv_base += kWordSizeInBytes;
  }
}


void Process::SaveContext(const Mips32State& state) {
  saved_registers_ = state.registers;
  saved_pc_ = state.pc;
  saved_memory_ = state.physical_memory->CreateCopy();
}


void Process::RestoreContext(Mips32State* state) {
  state->registers = saved_registers_;
  state->pc = saved_pc_;
  state->physical_memory->RestoreFromCopy(saved_memory_);
  free(saved_memory_);
}
