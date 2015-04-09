// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#ifndef EMULATOR_CONFIG_H_
#define EMULATOR_CONFIG_H_

#include <stdint.h>

#include "emulator/utils.h"

// Phyiscal memory available for a machine.
const intptr_t kPhysicalMemory = 32 * kMegaByte;

// Time slice for each process in miliseconds.
const intptr_t kTimeSlice = 100;

// Number of timer ticks per seconds.
const intptr_t kTicksPerSec = 100;

#endif  // EMULATOR_CONFIG_H_
