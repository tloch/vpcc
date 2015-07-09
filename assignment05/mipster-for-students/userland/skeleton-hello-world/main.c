// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#include "mipster.h"

char* str = "Enter character\n";

int main(int argc, char** argv) {
  mipster_syscall(4004, 1, (unsigned int)str, 16, 0);
  
  int *ptr = mipster_malloc(sizeof(int));
  *ptr = mipster_getchar();
  
  mipster_syscall(4004, 1, (unsigned int)ptr, 3, 0);
  
  mipster_exit(0);  
}