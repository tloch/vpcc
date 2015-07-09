// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#include "mipster.h"

/*
 typedef struct mipster_dump_raw_args {
 int code_addr; //int *code;
 int code_size;
 int name_addr; //char *name
 int name_size;
 } mipster_dump_raw_args_t;
 */

char* name = "a.mipster";

int main(int argc, char** argv) {
  
  
  int *ptr = mipster_malloc(4 * sizeof(int));
  ptr[0] = 123;
  ptr[1] = 456;
  ptr[2] = 789;
  ptr[3] = 1011;
  
  mipster_dump_raw_args_t args;
  args.code_addr = (int)ptr;
  args.code_size = 4 * sizeof(int); //code size in bytes!!
  args.name_addr = (int)name;
  args.name_size = 10; // a.mipster\0
  
  
  mipster_dump_raw(args);
  
  
  
  mipster_exit(0);  
}