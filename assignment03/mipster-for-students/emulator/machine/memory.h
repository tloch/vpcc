// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#ifndef EMULATOR_MACHINE_MEMORY_H_
#define EMULATOR_MACHINE_MEMORY_H_

#include <stdint.h>

// An abstraction for physical memory.
class Memory {
 public:
  explicit Memory(int32_t physical_memory_sz);

  template<typename T>
  inline void Write(uint32_t address, T value) {
    WriteBuffer(address, reinterpret_cast<char*>(&value), sizeof(value));
  }

  template<typename T>
  inline void Read(uint32_t address, T* value) const {
    ReadBuffer(address, reinterpret_cast<char*>(value), sizeof(*value));
  }

  void WriteBuffer(uint32_t address, char* buffer, int32_t bytes);
  void ReadBuffer(uint32_t address, char* buffer, int32_t bytes) const;

  inline int32_t Size() { return mem_size_; }

  char* CreateCopy();
  void RestoreFromCopy(char* raw);

 private:
  // The raw memory chunk.
  char* raw_chunk_;

  // Size of the raw memory chunk in bytes.
  int32_t mem_size_;

  // Base address used to allow address ranges starting at 0x0.
  uintptr_t base_;
};

#endif  // EMULATOR_MACHINE_MEMORY_H_
