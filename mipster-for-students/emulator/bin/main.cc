// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#include <stdint.h>
#include <stdio.h>
#include <gflags/gflags.h>

#include <cstdlib>
#include <list>
#include <tuple>

#include "emulator/config.h"
#include "emulator/flags.h"
#include "emulator/machine/mips32.h"
#include "emulator/os/os.h"

const std::list<std::tuple<std::string, std::string, int>>
CreateOperatingSystemInput(int start, int argc, char** argv) {
  std::list<std::tuple<std::string, std::string, int>> os_inputs;

  // Special case: Assignment 1 only requires users to specify the binary.
  if ((start < argc) && ((start + 3) > argc)) {
    // Assume only 1 arg, which is the path.
    std::string path(argv[start]);
    std::tuple<std::string, std::string, int> args(path, std::string(""), 31);
    os_inputs.push_back(args);
    return os_inputs;
  }

  // Otherwise:
  // mipster <binary-1> [ <arg-1> <prio-1> [ <binary-n> <argv-n> <prio-n> ] ]
  for (int i = start; i < argc; i+=3) {
    if ((i + 3) > argc) {
      fprintf(stderr, "incompatible command line format\n");
      abort();
    }
    std::string path(argv[i]);
    std::string argv_for_path(argv[i+1]);
    int prio = atoi(argv[i+2]);
    std::tuple<std::string, std::string, int> args(path, argv_for_path, prio);
    os_inputs.push_back(args);
  }
  return os_inputs;
}



int main(int argc, char** argv) {
  int start = google::ParseCommandLineFlags(&argc, &argv, true);

  std::list<std::tuple<std::string, std::string, int>> inputs =
      CreateOperatingSystemInput(start, argc, argv);

  OS::Configure(inputs);
  Mips32Machine machine = Mips32Machine::Create(kPhysicalMemory);
  
  
  machine.Run();

  return EXIT_SUCCESS;
}
