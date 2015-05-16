// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#include "mipster.h"

int mipster_syscall(int num, int arg_1, int arg_2, int arg_3, int arg_4) {
  int ret;
  asm("lw $v0, %1\n\t"
      "lw $a0, %2\n\t"
      "lw $a1, %3\n\t"
      "lw $a2, %4\n\t"
      "lw $a3, %5\n\t"
      "syscall\n\t"
      "sw $v0, %0\n\t"
      : "=m" (ret)
      : "m" (num), "m" (arg_1), "m" (arg_2), "m" (arg_3), "m" (arg_4));
  return ret;
}

void mipster_exit(int exit_code) {
  mipster_syscall(SYSCALL_EXIT, exit_code,
                  0 /* unused */, 0 /* unused*/, 0 /* unused */);
}

void *mipster_malloc(int size) {
  int addr = 0;
  if (size < 1) return (void*)0;
  mipster_syscall(SYSCALL_BUMP_POINTER, size, (int)&addr,
                  0 /* unused*/, 0 /* unused */);
  return (void*)addr;
}

int mipster_getchar() {
  int value = 0;
  mipster_syscall(SYSCALL_GETCHAR, (int)&value,
                  0 /* unused */, 0 /* unused*/, 0 /* unused */);
  return value;
}
