// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#ifndef MIPSTER_MIPSTER_H_
#define MIPSTER_MIPSTER_H_

// Constants that should be used from assembly.

#define SYSCALL_EXIT          4001

// internal undocumented syscalls
#define SYSCALL_BUMP_POINTER  5001
#define SYSCALL_GETCHAR       5002


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


#endif  // !__ASSEMBLY__
#endif  // MIPSTER_MIPSTER_H_
