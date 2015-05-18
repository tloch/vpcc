// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#ifndef EMULATOR_OS_OS_H_
#define EMULATOR_OS_OS_H_

#include <list>
#include <string>
#include <tuple>

#include "emulator/machine/arch.h"


const uint32_t kMipsSyscallExit          = 4001;
const uint32_t kMipsSyscallWrite         = 4004;
const uint32_t kMipsSyscallIOctl         = 4054;

//non-standard syscalls (starting at id=3000)
const uint32_t kMipsSyscallDumpElf       = 3000;
const uint32_t kMipsSyscallLoadElf       = 3001;
const uint32_t kMipsSyscallElfSize       = 3002;
const uint32_t kMipsSyscallBumpPointer   = 5001;
const uint32_t kMipsSyscallGetchar       = 5002;
const uint32_t kMipsSyscallCreateProcess = 5003;


class Process;
class Mips32Machine;

class OS {
 public:
   // Used to configure the operating system before the machine is even booted.
   // The configuration is then used in Init() to prepare the system.
  static void Configure(
      std::list<std::tuple<std::string, std::string, int>> inputs);

  // Getter used to get the only instance of the operating system.
  static OS& Get();

  // Unlike real world this method is called before the machine starts executing
  // code. It is used to set up initial processes.
  void Init();

  // Called after each CPU cycle.
  void TimerInterrupt();

  // Exception handler for exceptions thrown by the CPU.
  void ExceptionHandler(ExceptionType type);

  // Syscall handler.
  void Syscall();

  // Pagefault handler.
  void PageFault();

 private:
  explicit OS(
      std::list<std::tuple<std::string, std::string, int>> inputs);

  // Syscalls:
  //   Separated out in os/os_syscalls.cc
  void SyscallExit();
  void SyscallWrite();
  void SyscallIoctl();
  void SyscallDumpElf();
  void SyscallElfSize();
  void SyscallLoadElf();
  void SyscallBumpPointer();
  void SyscallGetchar();
  void SyscallCreateProcess();

  // The list of initial processes. Mipster does not fork from an idle process
  // but can be configured to start from a given set of processes.
  std::list<std::tuple<std::string, std::string, int>> inputs_;

  // The currently running process.
  Process* running_process_;

  // The list of all processes in the system.
  std::list<Process*> processes_;

  // Number of ticks since t he OS booted.
  int64_t ticks_;

  static OS* os_;
};

#endif  // EMULATOR_OS_OS_H_
