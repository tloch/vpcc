// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#ifndef MIPSTER_MIPSTER_H_
#define MIPSTER_MIPSTER_H_

// Constants that should be used from assembly.

#define SYSCALL_EXIT            4001

// internal undocumented syscalls
#define SYSCALL_BUMP_POINTER    5001
#define SYSCALL_GETCHAR         5002
#define SYSCALL_CREATE_PROCESS  5003
#define SYSCALL_DUMP_RAW        5004


// Only include the rest if we are not including from assembly.
#ifndef __ASSEMBLY__

// The entry point for the actual appliation.
int main();

//
// Mipster's own (very) tiny libc.
//

//
// Mips syscalls as described at
//   http://www.linux-mips.org/wiki/Syscall
//
// ABI: o32 (i.e.: syscall numbers in range 4000-4999)
//
int mipster_syscall(int nr, int arg_1, int arg_2, int arg_3, int arg_4);

// library calls
void mipster_exit(int exit_code);
void *mipster_malloc(int size);
int mipster_getchar();

// make sure all elements have the same size
typedef struct mipster_create_process_args {
  int code_addr; //int *code;
  int code_size;
  int entry_point; //int *entry_point;
  int name_addr; //char *name
  int name_size;
  int arguments_addr; //char *arguments;
  int arguments_size;
} mipster_create_process_args_t;

void mipster_create_process(mipster_create_process_args_t args);

// make sure all elements have the same size
typedef struct mipster_dump_raw_args {
  int code_addr; //int *code;
  int code_size;
  //int entry_point; //int *entry_point;
  int name_addr; //char *name
  int name_size;
  //int arguments_addr; //char *arguments;
  //int arguments_size;
} mipster_dump_raw_args_t;

void mipster_dump_raw(mipster_dump_raw_args_t args);


#endif  // !__ASSEMBLY__
#endif  // MIPSTER_MIPSTER_H_
