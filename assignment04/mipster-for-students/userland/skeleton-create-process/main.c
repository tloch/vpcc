// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#include "mipster.h"

/*
 typedef struct mipster_create_process_args {
 int code_addr; //int *code;
 int code_size;
 int entry_point; //int *entry_point;
 int name_addr; //char *name
 int name_size;
 int arguments_addr; //char *arguments;
 int arguments_size;
 } mipster_create_process_args_t;
 */

char* name = "foo";
char* argstring = "--help --me";

int main(int argc, char** argv) {
  
  
  int *ptr = mipster_malloc(4 * sizeof(int));
  ptr[0] = 123;
  ptr[1] = 456;
  ptr[2] = 789;
  ptr[3] = 1011;
  
  mipster_create_process_args_t args;
  args.code_addr = (int)ptr;
  args.code_size = 4 * 4;
  args.entry_point = 0; //offset in code array
  args.name_addr = (int)name;
  args.name_size = 4; // foo\0
  args.arguments_addr = (int)argstring;
  args.arguments_size = 12;
  
  
  mipster_create_process(args);
  
  
  
  mipster_exit(0);  
}