#ifndef ELF_H
#define ELF_H

#include "vm.h"

// ELF64 file header
struct Elf64Ehdr {
    unsigned char  ident[16];
    unsigned short type;
    unsigned short machine;
    unsigned int   version;
    unsigned long  entry;
    unsigned long  phoff;       // program header table offset
    unsigned long  shoff;       // section header table offset
    unsigned int   flags;
    unsigned short ehsize;
    unsigned short phentsize;
    unsigned short phnum;
    unsigned short shentsize;
    unsigned short shnum;
    unsigned short shstrndx;
};

// ELF64 program header
struct Elf64Phdr {
    unsigned int  type;
    unsigned int  flags;
    unsigned long offset;       // offset in file
    unsigned long vaddr;        // virtual address to load at
    unsigned long paddr;
    unsigned long filesz;       // bytes in file
    unsigned long memsz;        // bytes in memory (>= filesz, extra = BSS)
    unsigned long align;
};

// ELF constants
#define ELF_MAGIC  0x464c457fU  // \x7fELF little-endian
#define ET_EXEC    2
#define EM_RISCV   0xf3
#define PT_LOAD    1
#define PF_X       1
#define PF_W       2
#define PF_R       4

// Load ELF into page table pt. Returns entry point on success, 0 on failure.
unsigned long elf_load(const unsigned char *data, pagetable_t pt);

#endif
