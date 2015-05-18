// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#ifndef EMULATOR_OS_LOADER_H_
#define EMULATOR_OS_LOADER_H_

#include <string>

class Memory;

namespace mipster {

// A loader used to load ELF segments.
class Loader {
 public:
  uint32_t ElfFileSize(const char* file_path);
  uint32_t LoadElf_32LE(const std::string& file_path, Memory* memory,
                        uint32_t *memory_offset, uint32_t *end_address);
};
  
class Dumper {
 public:
  // on success, returns size (aka number of bytes written to ELF file)
  uint32_t DumpElf_32LE(uint32_t start_address, uint32_t size,
                    uint32_t entry_point, const char* file_path);
};

}  // namespace mipster

#endif  // EMULATOR_OS_LOADER_H_
