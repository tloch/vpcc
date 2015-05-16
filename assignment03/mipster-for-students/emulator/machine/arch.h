// Copyright (c) 2014, the mipser Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#ifndef EMULATOR_MACHINE_ARCH_H_
#define EMULATOR_MACHINE_ARCH_H_

enum ExceptionType {
  kAddressError,
  kInterOverflow,
  kReservedInstruction,
};

#endif  // EMULATOR_MACHINE_ARCH_H_
