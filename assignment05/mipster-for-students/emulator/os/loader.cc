// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#include "os/loader.h"

#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <gelf.h>
#include <gflags/gflags.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "emulator/machine/memory.h"
#include "emulator/machine/mips32.h"
#include "emulator/utils.h"

DEFINE_bool(debug_loader, false, "Printer loader information.");

#define LM (BLUE "[L]")

// Constants for checking segment types.
#define P_TYPE_LOAD 0x1

namespace mipster {
  
uint32_t Loader::LoadRaw(const std::string& file_path, Memory* memory,
                         uint32_t *memory_offset, uint32_t *end_address) {
  //memory_offset is currently unused for loading raw executables
  
  const char* c_path = file_path.c_str();
  if (FLAGS_debug_loader) {
    DebugModule(LM, "trying to load file: %s", c_path);
  }
  
  std::ifstream rawfile;
  rawfile.open(c_path, std::ios::in | std::ios::binary | std::ios::ate);
  
  if (!rawfile.is_open()) {
    ErrorOut(EXIT_FAILURE, LM, "open '%s' failed", c_path);
  }
  std::streampos size = rawfile.tellg();
  rawfile.seekg (0, std::ios::beg);
  char *codeblock = new char[size];
  rawfile.read(codeblock, size);
  rawfile.close();

  memory->WriteBuffer(0, codeblock, size);
  *end_address = size;

  delete [] codeblock;
  
  return 0;
}
  
  
uint32_t Loader::ElfFileSize(const char* file_path) {
    struct stat stat_buf;
    int rc = stat(file_path, &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

// TODO: currently, the loader sets an elf file for immediate execution
  // we need a way to tell mipster where to put segments from elf.
  // The the regular case (for execution) it works the same way as it does now.
uint32_t Loader::LoadElf_32LE(const std::string& file_path, Memory* memory,
                              uint32_t *memory_offset, uint32_t *end_address) {
  if (elf_version(EV_CURRENT) == EV_NONE) {
    ErrorOut(EXIT_FAILURE, LM,
             "elf library initialization failed: %s", elf_errmsg(-1));
  }

  const char* c_path = file_path.c_str();
  if (FLAGS_debug_loader) {
    DebugModule(LM, "trying to load file: %s", c_path);
  }
  int fd = open(c_path, O_RDONLY);
  if (fd == -1)  {
    ErrorOut(EXIT_FAILURE, LM, "open '%s' failed", c_path);
  }

  Elf* e = elf_begin(fd, ELF_C_READ, NULL);
  if (e == NULL) {
    ErrorOut(EXIT_FAILURE, LM, "elf_begin() failed: %s", elf_errmsg(-1));
  }

  if (elf_kind(e) != ELF_K_ELF) {
    ErrorOut(EXIT_FAILURE, LM, "'%s' is not an ELF object", c_path);
  }

  GElf_Ehdr ehdr;
  if (gelf_getehdr(e, &ehdr) == NULL) {
    ErrorOut(EXIT_FAILURE, LM, "get_ehdr() failed: %s", elf_errmsg(-1));
  }
  
  if (gelf_getclass(e) != ELFCLASS32) {
    ErrorOut(EXIT_FAILURE, LM, "only supporting 32 bit");
  }
  size_t ident;
  elf_getident(e, &ident);
  // Manually checking endianess through ident bit 10.
  if ((ident & 0x0A) != 0) {
    ErrorOut(EXIT_FAILURE, LM, "only supporting little endian");
  }

  uint64_t entry_point = static_cast<uintptr_t>(ehdr.e_entry);
  if (FLAGS_debug_loader) {
    DebugModule(LM, "entry point at %p", reinterpret_cast<void*>(entry_point));
  }

  size_t n;
  if (elf_getphdrnum(e, &n) != 0) {
    ErrorOut(EXIT_FAILURE, LM, "elf_getphdrnum() failed: %s", elf_errmsg(-1));
  }
  
  if (FLAGS_debug_loader) {
    DebugModule(LM, "phdrnum: %d", n);
  }


  GElf_Phdr phdr;
  FILE* fp = fdopen(fd, "r");
  if (fp == NULL) {
    ErrorOut(EXIT_FAILURE, LM, "fdopen() failed");
  }
  for (size_t i = 0; i < n; ++i) {
    if (gelf_getphdr(e, i, &phdr) != &phdr) {
      ErrorOut(EXIT_FAILURE, LM, "gelf_getphdr() failed: %s", elf_errmsg(-1));
    }
    if (static_cast<uintmax_t>(phdr.p_type) == P_TYPE_LOAD) {
      uint64_t offset = static_cast<uint64_t>(phdr.p_offset);
      uint64_t file_sz = static_cast<uint64_t>(phdr.p_filesz);
      uint64_t mem_sz = static_cast<uint64_t>(phdr.p_memsz);
      uintptr_t address = static_cast<uintptr_t>(phdr.p_vaddr);
      if (i == 0) *memory_offset = address; //tell caller where first code starts
      if (FLAGS_debug_loader) {
        DebugModule(LM, "segment info:\n"
                        "\t  file offset: %" PRIx64 "\n"
                        "\t  file size: %" PRId64 "\n"
                        "\t  memory size: %" PRId64 "\n"
                        "\t  address: %p",
                    offset,
                    file_sz,
                    mem_sz,
                    reinterpret_cast<void*>(address));
      }
      
      if (fseek(fp, offset, SEEK_SET) < 0) {
        ErrorOut(EXIT_FAILURE, LM,
                 "fseek() failed: offset: %" PRIx64 "", offset);
      }
      char* buffer = static_cast<char*>(calloc(mem_sz, sizeof(*buffer)));
      size_t read = fread(buffer, sizeof(*buffer), file_sz, fp);
      if (read != file_sz) {
        ErrorOut(EXIT_FAILURE, LM,
             "fread: number of bytes read mismatching. "
             "expected: %" PRIx64 ", read: %lu",
             file_sz, read);
      }
      memory->WriteBuffer(address, buffer, mem_sz);
      // tell caller the end address of the code to start the heap
      *end_address = address + mem_sz;
      free(buffer);
    }
  }

  elf_end(e);
  fclose(fp);
  close(fd);
  return static_cast<uint32_t>(entry_point);
}

/*
 * writes a memory block starting at "start_address"
 * writes up to "size" bytes
 * sets the entry point to address
 */
uint32_t Dumper::DumpElf_32LE(uint32_t start_address, uint32_t size,
                          uint32_t entry_point, const char *file_path) {

  
  char string_table[] = {
    /* Offset 0 */ '\0',
    /* Offset 1 */ '.', 't', 'e', 'x', 't', '\0',
    /* Offset 7 */ '.', 's', 'h', 's', 't', 'r', 't', 'a', 'b', '\0',
    /*Terminater*/ '\0'
  };
  
  if (elf_version(EV_CURRENT) == EV_NONE) {
    ErrorOut(EXIT_FAILURE, LM,
             "elf library initialization failed: %s", elf_errmsg(-1));
  }

  if (FLAGS_debug_loader) {
    DebugModule(LM, "trying to dump file: %s", file_path);
  }
  int fd = open(file_path, O_RDWR | O_CREAT, 0777);
  if (fd == -1)  {
    ErrorOut(EXIT_FAILURE, LM, "opening '%s' for writing failed", file_path);
  }
  
  Elf *e = elf_begin(fd, ELF_C_WRITE, NULL);
  if (e  == NULL) {
    ErrorOut(EXIT_FAILURE, LM, "elf_begin() failed: '%s'", elf_errmsg(-1));
  }
  
  Elf32_Ehdr *ehdr = elf32_newehdr(e);
  if (ehdr  == NULL) {
    ErrorOut(EXIT_FAILURE, LM, "elf32_newehdr() failed: '%s'", elf_errmsg(-1));
  }
  
  ehdr->e_ident[EI_DATA] = ELFDATA2LSB;
  ehdr->e_machine = EM_MIPS;
  ehdr->e_type = ET_EXEC;
  ehdr->e_version = EV_CURRENT;
  ehdr->e_shstrndx = 2;
  
  Elf32_Phdr *phdr = elf32_newphdr(e, 1);
  if (phdr  == NULL) {
    ErrorOut(EXIT_FAILURE, LM, "elf32_newphdr() failed: '%s'", elf_errmsg(-1));
  }

  // text section
  Elf_Scn *scn = elf_newscn(e);
  if (scn  == NULL) {
    ErrorOut(EXIT_FAILURE, LM, "elf_newscn() failed: '%s'", elf_errmsg(-1));
  }
  
  Elf_Data *data = elf_newdata(scn);
  if (data  == NULL) {
    ErrorOut(EXIT_FAILURE, LM, "elf_newdata() failed: '%s'", elf_errmsg(-1));
  }
  
  char *buffer = new char[size];
  Memory *mem = Mips32Machine::Get().GetMemory();
  mem->ReadBuffer(start_address, buffer, size);
  
  data->d_align = 4;
  data->d_off = 0LL;
  data->d_buf = buffer;
  data->d_type = ELF_T_WORD;
  data->d_size = size;
  data->d_version = EV_CURRENT;
  
  
  Elf32_Shdr *shdr = elf32_getshdr(scn);
  if (shdr  == NULL) {
    ErrorOut(EXIT_FAILURE, LM, "elf32_getshdr() failed: '%s'", elf_errmsg(-1));
  }
  shdr->sh_name = 1;
  shdr->sh_type = SHT_PROGBITS;
  shdr->sh_flags = SHF_ALLOC;
  shdr->sh_entsize = 0;
  
  //string section
  scn = elf_newscn(e);
  if (scn  == NULL) {
    ErrorOut(EXIT_FAILURE, LM, "elf_newscn() failed: '%s'", elf_errmsg(-1));
  }
  
  data = elf_newdata(scn);
  if (data  == NULL) {
    ErrorOut(EXIT_FAILURE, LM, "elf_newdata() failed: '%s'", elf_errmsg(-1));
  }
  
  data->d_align = 1;
  data->d_buf = string_table;
  data->d_off = 0LL;
  data->d_size = sizeof(string_table);
  
  shdr = elf32_getshdr(scn);
  if (shdr  == NULL) {
    ErrorOut(EXIT_FAILURE, LM, "elf32_getshdr() failed: '%s'", elf_errmsg(-1));
  }
  shdr->sh_name = 7;
  shdr->sh_type = SHT_STRTAB;
  shdr->sh_flags = SHF_STRINGS | SHF_ALLOC;
  shdr->sh_entsize = 0;
  
  
  
  //elf_setshstrndx(e, elf_ndxscn(scn));
  
  //update elf in memory
  if (elf_update(e, ELF_C_NULL) < 0) {
    ErrorOut(EXIT_FAILURE, LM, "elf_update(C_NULL) failed: '%s'", elf_errmsg(-1));
  }
  
  //write program header for executable section
  phdr->p_type = PT_LOAD;
  phdr->p_offset = 0; //ehdr->e_phoff;
  phdr->p_filesz = size; // elf32_fsize(ELF_T_PHDR, 1, EV_CURRENT);
  phdr->p_memsz = size;
  phdr->p_vaddr = entry_point;
  
  
  size_t ehdrsz = elf32_fsize(ELF_T_EHDR, 1, EV_CURRENT);
  size_t phdrsz = elf32_fsize(ELF_T_PHDR, 1, EV_CURRENT);
  ehdr->e_entry = entry_point + ehdrsz + phdrsz;
  
  elf_flagphdr(e, ELF_C_SET, ELF_F_DIRTY);
  
  //update elf in file
  if (elf_update(e, ELF_C_WRITE) < 0) {
    ErrorOut(EXIT_FAILURE, LM, "elf_update(C_WRITE) failed: '%s'", elf_errmsg(-1));
  }
  
  elf_end(e);
  
  delete [] buffer;
  
  close(fd);

  return size;
}

  
  
  
  
  
  
  
  
  
  
  
}  // namespace mipster

#undef LM
