# Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
# Please see the AUTHORS file for details.  Use of this source code is governed
# by a BSD license that can be found in the LICENSE file.

#define __ASSEMBLY__
#include "mipster.h"

.text
.globl __start
__start:
  # No need to save return address.
  # No need to save old frame pointer.
  # No register save area, thus no need for double word alignment.

  move $fp, $sp     # current frame pointer = stack pointer

  # Call: int main(int argc, char** argv)
  # For 2 arguments we need 2 * 4 bytes = 8 bytes
  addiu $sp, $sp, -8
  lw $a0, 8($sp)
  la $a1, 12($sp)
  jal main          # jump and link (writing return address into $ra)
  nop               # branch delay slot

  move $t0, $v0     # save return value
  li $v0, SYSCALL_EXIT      # exit syscall number
  move $a0, $t0     # exit code
  syscall           # syscall
