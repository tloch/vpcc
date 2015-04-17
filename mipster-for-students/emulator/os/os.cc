// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#include "os/os.h"

#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <gflags/gflags.h>
#include <stdio.h>

#include <tuple>

#include "emulator/machine/mips32.h"
#include "emulator/os/loader.h"
#include "emulator/os/process.h"
#include "emulator/utils.h"

DEFINE_bool(debug_os, false, "Print OS information.");
DEFINE_bool(torture_mode, false, "Skip the instruction causing an int overflow "
                                 "and one more. (used for torture test)");
DECLARE_bool(debug_syscalls);


#define OSM (BLUE "[OS]")

// Singleton instance.
OS* OS::os_;


OS& OS::Get() {
  return *os_;
}


void OS::Configure(
    std::list<std::tuple<std::string, std::string, int>> inputs) {
  os_ = new OS(inputs);
}


OS::OS(
    std::list<std::tuple<std::string, std::string, int>> inputs) {
  inputs_ = inputs;
  ticks_ = 0;
}


void OS::Init() {
  if (FLAGS_debug_os) {
    DebugModule(OSM, "initializing operating system");
  }
  Mips32State& ms = Mips32Machine::Get().MachineState();
  mipster::Loader loader;

  for (auto it = inputs_.begin(); it != inputs_.end(); ++it) {
    const std::string program_name = std::get<0>(*it);
    const std::string raw_argv = std::get<1>(*it);
    const int32_t prio = std::get<2>(*it);
    const int32_t stack_address =
        Mips32Machine::Get().MachineState().physical_memory->Size() -
        kArgumentsSpace;
    const std::string extended_argv = program_name + " " + raw_argv;
    Process* p = new Process(stack_address, prio, extended_argv);
    uint32_t offset;
    const int32_t entry_point = loader.LoadElf_32LE(
        program_name, Mips32Machine::Get().MachineState().physical_memory,
        &offset);
    ms.registers.sp = p->stack_addr();
    ms.pc = entry_point;
    // TODO(martin): save heap starting address in some register
    p->SaveContext(ms);
    processes_.push_back(p);
  }

  running_process_ = processes_.front();
  running_process_->RestoreContext(&ms);
}


void OS::TimerInterrupt() {
  ticks_++;
}


void OS::PageFault() {
}


void OS::ExceptionHandler(ExceptionType type) {
  if (FLAGS_debug_os) {
    printf("OS: exception handler\n");
  }
  switch (type) {
    // All those exceptions should kill the currently running process.
    // Implement this once we have multi process support.
    case kAddressError:
      if (FLAGS_torture_mode) {
          Mips32Machine::Get().Recover(2);
          UNREACHABLE();
      }
      errx(EXIT_FAILURE, "address error");
      break;
    case kInterOverflow:
      if (FLAGS_torture_mode) {
          Mips32Machine::Get().Recover(2);
          UNREACHABLE();
      }
      errx(EXIT_FAILURE, "integer overflow");
      break;
    case kReservedInstruction:
      errx(EXIT_FAILURE, "reserved instruction");
      break;
    default:
      errx(EXIT_FAILURE, "unknown exception type");
  }
}


void OS::Syscall() {
  Mips32Machine& m = Mips32Machine::Get();
  int32_t syscall_nr = m.MachineState().registers.v0;

  if (FLAGS_debug_syscalls) {
    DebugModule(OSM, "syscall: %" PRId32 "", syscall_nr);
  }

  /* https://android.googlesource.com/platform/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.6/+/ics-plus-aosp/share/gdb/syscalls/mips-o32-linux.xml */
  // See os/os_syscall.cc for implementations.
  switch (syscall_nr) {
    case 1:
    case kMipsSyscallExit: // syscall ids according to MIPS ABI
      SyscallExit();
      break;
    case kMipsSyscallRead:
      SyscallRead();
      break;
    case kMipsSyscallWrite:
      SyscallWrite();
      break;
    case kMipsSyscallIOctl:
      SyscallIoctl();
      break;
    case kMipsSyscallDumpElf:
      SyscallDumpElf();
      break;
    case kMipsSyscallElfSize:
      SyscallElfSize();
      break;
    default:
      ErrorOut(EXIT_FAILURE, OSM, "syscall %" PRId32 " not implemented",
               syscall_nr);
  }
}

#undef OSM
