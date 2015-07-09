// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#include "os/os.h"

#include <gflags/gflags.h>
#include <iostream>
#include <fstream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "emulator/config.h"
#include "emulator/machine/mips32.h"
#include "emulator/os/loader.h"
#include "emulator/os/process.h"
#include "emulator/utils.h"

DEFINE_bool(debug_syscalls,       false,
            "Print debugging information for syscalls.");
DEFINE_bool(stop_before_syscall,  false,
            "Stop *before* executing a syscall.");

#define OSM (BLUE "[OS]")

using namespace std;

static void DebugSyscall(const char* format, ...) {
  if (FLAGS_debug_syscalls) {
    va_list argptr;
    va_start(argptr, format);
    VDebugModule(OSM, format, argptr);
    va_end(argptr);
  }
  if (FLAGS_stop_before_syscall) {
    printf("Stopping before executing the syscall. Press 'c' to continue.\n");
    int c = 0;
    while ((c = getchar()) != 'c') {}
  }
}


//
// Syscall ABI:
//     http://www.linux-mips.org/wiki/Syscall
//

void OS::SyscallExit() {
  // man 2 exit
  //
  //   The function _exit() terminates the calling process "immediately".
  //   Any open file descriptors belonging to the process are closed; any
  //   children of the process are inherited by process 1, init, and the
  //   process's parent is sent a SIGCHLD signal.
  //
  //   The value status is returned to the parent process as the process's
  //   exit status, and can be collected using one of the wait(2) family of
  //   calls.

  Mips32Machine& m = Mips32Machine::Get();
  int32_t return_code = m.MachineState().registers.a0;
  delete running_process_;
  running_process_ = NULL;
  DebugSyscall("exit %d", return_code);
  DebugModule(OSM, "terminated with %d", return_code);
  exit(EXIT_SUCCESS);
}


void OS::SyscallWrite() {
  Mips32Machine& m = Mips32Machine::Get();
  int32_t fd = m.MachineState().registers.a0;
  uint32_t address = static_cast<uint32_t>(m.MachineState().registers.a1);
  int32_t bytes = m.MachineState().registers.a2;
  DebugSyscall("write %d %p %d",
               fd,
               reinterpret_cast<void*>(address),
               bytes);
  char* buffer =  new char[bytes+1];
  m.MachineState().physical_memory->ReadBuffer(address, buffer, bytes);
  buffer[bytes] = 0;
  printf("%s", buffer);
  fflush(stdout);
}


void OS::SyscallIoctl() {
  Mips32Machine& m = Mips32Machine::Get();
  int32_t d = m.MachineState().registers.a0;
  int32_t request = m.MachineState().registers.a1;
  DebugSyscall("ioctl fd:%d request: %x",
               d, request);
  // Would need to check wether fd is open.
  switch (request) {
    case 0x540D:  // = TIOCNXCL
      break;
    default:
      ErrorOut(EXIT_FAILURE, OSM,  "unknown ioctl request: %" PRId32, request);
  }
}


// this is the same data structure as in mipster-libc
// make sure all elements have the same fixed size
typedef struct mipster_create_process_args {
  int code_addr; //int *code;
  int code_size;
  int entry_point; //int *entry_point;
  int name_addr; //char *name
  int name_size;
  int arguments_addr; //char *arguments;
  int arguments_size;
} mipster_create_process_args_t;

void OS::SyscallCreateProcess() {
  
  Mips32Machine& m = Mips32Machine::Get();
  Mips32State& ms = m.MachineState();
  uint32_t arg_address = ms.registers.a0;
  mipster_create_process_args_t args;
  
  
  // copy argument structure from user space
  ms.physical_memory->ReadBuffer(arg_address,
                                 reinterpret_cast<char*>(&args),
                                 sizeof(mipster_create_process_args));
  
  /*
  std::cout << "address " << arg_address << std::endl;
  std::cout << "code " << args.code_addr << std::endl;
  std::cout << "code_size " << args.code_size << std::endl;
  std::cout << "entry_point " << args.entry_point << std::endl;
  std::cout << "name " << args.name_addr << std::endl;
  std::cout << "name_size " << args.name_size << std::endl;
  std::cout << "arguments " << args.arguments_addr << std::endl;
  std::cout << "arguments_size " << args.arguments_size << std::endl;
  */
  
  // copy strings from userspace
  char *name = new char[args.name_size];
  char *arguments = new char[args.arguments_size];
  int32_t *code = new int32_t[args.code_size];
  
  ms.physical_memory->ReadBuffer(args.name_addr, name, args.name_size);
  ms.physical_memory->ReadBuffer(args.arguments_addr, arguments,
                                 args.arguments_size);
  ms.physical_memory->ReadBuffer(args.code_addr,
                                 reinterpret_cast<char*>(code),
                                 args.code_size);

  /*
  std::cout << "name.string " << name << std::endl;
  std::cout << "arguments.string " << arguments << std::endl;
  */
  
  DebugSyscall("createProcess %s %s",
               name,
               arguments);
  
  std::string raw_name(name);
  std::string raw_args(arguments);
  std::string extended_args = raw_name + " " + raw_args;
  
  // Create new instance of Memory
  Memory *mem = new Memory(kPhysicalMemory);
  
  // Set the stack pointer 4k below top of memory
  uint32_t stack_address = mem->Size() - kArgumentsSpace;
  // Heap starts after code
  uint32_t heap_address = args.code_size;
  
  // create a new instance of Process
  int32_t my_prio = running_process_->priority();
  Process *p = new Process(stack_address, my_prio, extended_args);
  
  delete [] name;
  delete [] arguments;

  // Copy code and globals and static to new instance
  // Assume: code starts at 0x0
  mem->WriteBuffer(0, reinterpret_cast<char*>(code), args.code_size);
  delete [] code;
  
  // save the context of the calling process (still active)
  running_process_->SaveContext(ms);
  
  // Setup Machine state (overwriting old state)
  // 1: init register. stack pointer, PC, heap register....
  // 2: init physical memory. pointer to new instance of Memory
  // 3: set PC to entry point
  ms.registers.sp = p->stack_addr();
  ms.pc = args.entry_point - kInstructionSize;
  ms.registers.k1 = heap_address;
  ms.physical_memory = mem;

  // set running process to new instance of Process
  processes_.push_back(p);
  running_process_ = p;
}



void OS::SyscallBumpPointer() {
  Mips32Machine& m = Mips32Machine::Get();
  Mips32State& ms = m.MachineState();
  int32_t size = ms.registers.a0;
  int32_t address = ms.registers.a1;
  // TODO: parameter sanity checks
  DebugSyscall("bumpPointer %d %p",
               size,
               address);

  int32_t old_bump_pointer = ms.registers.k1;
  if (old_bump_pointer + size >= ms.registers.sp) {
    // heap collides with stack: exit
    ErrorOut(EXIT_FAILURE, OSM,  "heap overflow");
  }
  
  // pass bump pointer to caller
  ms.physical_memory->Write(address, old_bump_pointer);
  
  // move bump pointer
  ms.registers.k1 += size;
}

void OS::SyscallGetchar() {
  Mips32Machine& m = Mips32Machine::Get();
  Mips32State& ms = m.MachineState();
  
  uint32_t return_address = static_cast<uint32_t>(ms.registers.a0);
  
  DebugSyscall("getchar %d", return_address);
  
  char character = getchar();
  
  ms.physical_memory->Write(return_address, static_cast<int>(character));
}

void OS::SyscallDumpElf() {
  Mips32Machine& m = Mips32Machine::Get();
  
  uint32_t start_address = static_cast<uint32_t>(m.MachineState().registers.a0);
  uint32_t size = static_cast<uint32_t>(m.MachineState().registers.a1);
  uint32_t entry_offset = static_cast<uint32_t>(m.MachineState().registers.a2);
  uint32_t file_name_address =
    static_cast<uint32_t>(m.MachineState().registers.a3);
  
  DebugSyscall("dumpElf %d %d %d %d",
               start_address, size, entry_offset, file_name_address);
  
  // TODO: only read until \0
  char *file_name_buffer = new char[4096];
  m.MachineState().physical_memory->ReadBuffer(file_name_address,
                                               file_name_buffer, 4096);
  
  //std::cout << buffer << std::endl;
  
  mipster::Dumper dumper;
  dumper.DumpElf_32LE(start_address, size, entry_offset, file_name_buffer);
  
  delete [] file_name_buffer;
}


// this is the same data structure as in mipster-libc
// make sure all elements have the same fixed size
typedef struct mipster_dump_raw_args {
  int code_addr; //int *code;
  int code_size;
  int name_addr; //char *name
  int name_size;
} mipster_dump_raw_args_t;



void OS::SyscallDumpRaw() {

  Mips32Machine& m = Mips32Machine::Get();
  Mips32State& ms = m.MachineState();
  uint32_t arg_address = ms.registers.a0;
  mipster_dump_raw_args_t args;
  
  
  // copy argument structure from user space
  ms.physical_memory->ReadBuffer(arg_address,
                                 reinterpret_cast<char*>(&args),
                                 sizeof(mipster_dump_raw_args));
  
  /*
  std::cout << "address " << arg_address << std::endl;
  std::cout << "code " << args.code_addr << std::endl;
  std::cout << "code_size " << args.code_size << std::endl;
  std::cout << "name " << args.name_addr << std::endl;
  std::cout << "name_size " << args.name_size << std::endl;
  */
  
  // copy strings from userspace
  char *name = new char[args.name_size];
  char *code = new char[args.code_size];
  
  ms.physical_memory->ReadBuffer(args.name_addr, name, args.name_size);
  ms.physical_memory->ReadBuffer(args.code_addr, code, args.code_size);
  
  
  //std::cout << "name.string " << name << std::endl;
  
  
  DebugSyscall("dumpRaw %s", name);
  
  ofstream rawfile;
  rawfile.open(name, ios::out | ios::binary | ios::trunc);
  
  if (!rawfile.is_open()) {
    ErrorOut(EXIT_FAILURE, OSM,  "unable to open raw dump: %s", name);
  }
  
  rawfile.write(code, args.code_size);
  rawfile.close();
  
  delete [] name;
  delete [] code;
}



void OS::SyscallLoadElf() {
  Mips32Machine& m = Mips32Machine::Get();
  
  uint32_t code_buffer_address =
    static_cast<uint32_t>(m.MachineState().registers.a0);
  uint32_t size =
    static_cast<uint32_t>(m.MachineState().registers.a1);
  uint32_t entry_offset_address =
    static_cast<uint32_t>(m.MachineState().registers.a2);
  uint32_t file_name_address =
    static_cast<uint32_t>(m.MachineState().registers.a3);
  
  DebugSyscall("dumpElf %d %d %d %d",
               code_buffer_address, size,
               entry_offset_address, file_name_address);
  
  // TODO: only read until \0
  char *file_name_buffer = new char[4096];
  m.MachineState().physical_memory->ReadBuffer(file_name_address,
                                               file_name_buffer, 4096);
  
  //std::cout << buffer << std::endl;
  
  mipster::Loader loader;
  uint32_t code_offset;
  uint32_t end_address;
  uint32_t entry_point =
    loader.LoadElf_32LE(file_name_buffer, m.MachineState().physical_memory,
                        &code_offset, &end_address);
  delete [] file_name_buffer;
  
//  *entry_offset_address = entry_point;
  m.MachineState().physical_memory->Write(entry_offset_address, entry_point);
  
  
  //TODO: Refactor Loader to allow loading to specific address
  
  
  //m.MachineState().physical_memory->ReadBuffer
  
}


void OS::SyscallElfSize() {
  Mips32Machine& m = Mips32Machine::Get();
  
  uint32_t file_name_address =
    static_cast<uint32_t>(m.MachineState().registers.a0);
  uint32_t result_address =
    static_cast<uint32_t>(m.MachineState().registers.a1);
  
  DebugSyscall("dumpElf %d",
               file_name_address);
  
  // TODO: only read until \0
  char *file_name_buffer = new char[4096];
  m.MachineState().physical_memory->ReadBuffer(file_name_address,
                                               file_name_buffer, 4096);
  
  // use loader to obtain ELF file size
  mipster::Loader loader;
  uint32_t file_size = loader.ElfFileSize(file_name_buffer);
  delete [] file_name_buffer;
  
  //write file size back to user space
  m.MachineState().physical_memory->Write(result_address, file_size);
  
  
}


#undef OSM
