// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#include "emulator/machine/memory.h"

#include <err.h>
#include <gflags/gflags.h>
#include <stdlib.h>

#include "emulator/machine/mips32.h"
#include "emulator/utils.h"

DEFINE_bool(debug_memory, false, "Print memory information");

#define MRM (GREEN "[M-R]")
#define MWM (GREEN "[M-W]")


static int32_t ceiled_division(int32_t dividend, int32_t divisor) {
  return (dividend + (divisor - 1))/divisor;
}


Memory::Memory(int32_t physical_memory_sz) {
  int err = posix_memalign(
      reinterpret_cast<void**>(&raw_chunk_), 0x10000, physical_memory_sz);
  if (err != 0) {
    errx(EXIT_FAILURE, "posix_memalign failed");
  }
  memset(raw_chunk_, 0, physical_memory_sz);
  base_ = reinterpret_cast<uintptr_t>(raw_chunk_);
  mem_size_ = physical_memory_sz;
}


void Memory::WriteBuffer(uint32_t address, char* buffer, int32_t bytes) {
  if ((address + bytes) > static_cast<uint32_t>(mem_size_)) {
    ErrorOut(EXIT_FAILURE, MRM,
             "read out of bounds. address: %p, bytes: %" PRId32 "",
             reinterpret_cast<void*>(address), bytes);
  }
  memcpy(reinterpret_cast<void*>(base_ + address), buffer, bytes);
  if (FLAGS_debug_memory) {
    // For printing we align to the next lower word.
    const uint32_t print_base = address & 0xFFFFFFFC;
    const int32_t print_bytes =  address & 0x3;
    for (int i = 0;
         i < ceiled_division(bytes + print_bytes, kInstructionSize);
         i++) {
      DebugModule(MWM, "%p %08x",
                  reinterpret_cast<void*>(print_base + i * kInstructionSize),
                  reinterpret_cast<int32_t*>(base_ + print_base)[i]);
    }
  }
}


void Memory::ReadBuffer(uint32_t address, char* buffer, int32_t bytes) const {
  if ((address + bytes) > static_cast<uint32_t>(mem_size_)) {
    ErrorOut(EXIT_FAILURE, MWM,
             "read out of bounds. address: %p, bytes: %" PRId32 "",
             reinterpret_cast<void*>(address), bytes);
  }
  memcpy(buffer, reinterpret_cast<void*>(base_ + address), bytes);
  if (FLAGS_debug_memory) {
    // For printing we align to the next lower word.
    const uint32_t print_base = address & 0xFFFFFFC;
    const int32_t print_bytes =  address & 0x3;
    for (int i = 0;
         i < ceiled_division(bytes + print_bytes, kInstructionSize);
         i++) {
      DebugModule(MRM, "%p %08x",
                  reinterpret_cast<void*>(print_base + i * kInstructionSize),
                  reinterpret_cast<int32_t*>(base_ + print_base)[i]);
    }
  }
}


char* Memory::CreateCopy() {
  char* copy;
  int err = posix_memalign(
      reinterpret_cast<void**>(&copy), 0x10000, mem_size_);
  if (err != 0) {
    errx(EXIT_FAILURE, "posix_memalign failed");
  }
  memcpy(copy, raw_chunk_, mem_size_);
  return copy;
}


void Memory::RestoreFromCopy(char *copy) {
  memcpy(raw_chunk_, copy, mem_size_);
}


#undef MRM
#undef MWM
