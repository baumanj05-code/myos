#include "elf.h"
#include "pmem.h"
#include "vm.h"
#include "print.h"

static void memcpy(void *dst, const void *src, unsigned long n) {
    char *d = (char *)dst;
    const char *s = (const char *)src;
    while (n--) *d++ = *s++;
}

static void memzero(void *dst, unsigned long n) {
    char *d = (char *)dst;
    while (n--) *d++ = 0;
}

static unsigned long min(unsigned long a, unsigned long b) { return a < b ? a : b; }
static unsigned long max(unsigned long a, unsigned long b) { return a > b ? a : b; }

// Convert ELF segment flags (PF_*) to PTE flags
static unsigned long elf_flags_to_pte(unsigned int flags) {
    unsigned long pte = PTE_U;
    if (flags & PF_R) pte |= PTE_R;
    if (flags & PF_W) pte |= PTE_W;
    if (flags & PF_X) pte |= PTE_X;
    return pte;
}

unsigned long elf_load(const unsigned char *data, pagetable_t pt) {
    auto *hdr = (const Elf64Ehdr *)data;

    // Validate
    if (*(unsigned int *)hdr->ident != ELF_MAGIC) {
        kprintf("elf: bad magic\n");
        return 0;
    }
    if (hdr->type != ET_EXEC || hdr->machine != EM_RISCV) {
        kprintf("elf: not a RISC-V executable\n");
        return 0;
    }

    // Walk program headers
    for (int i = 0; i < hdr->phnum; i++) {
        auto *ph = (const Elf64Phdr *)(data + hdr->phoff + i * hdr->phentsize);

        if (ph->type != PT_LOAD || ph->memsz == 0) continue;

        unsigned long va     = ph->vaddr;
        unsigned long filesz = ph->filesz;
        unsigned long memsz  = ph->memsz;
        unsigned long flags  = elf_flags_to_pte(ph->flags);

        // Page-align the range
        unsigned long va_start = va & ~(PAGE_SIZE - 1);
        unsigned long va_end   = (va + memsz + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

        kprintf("elf: load segment va=0x%x size=0x%x flags=0x%x\n",
                (unsigned int)va, (unsigned int)memsz, (unsigned int)ph->flags);

        for (unsigned long pva = va_start; pva < va_end; pva += PAGE_SIZE) {
            // Allocate a physical page and zero it (handles BSS automatically)
            unsigned long pa = (unsigned long)pmem_alloc();
            memzero((void *)pa, PAGE_SIZE);

            // Copy file bytes that fall within this page
            // Overlap of [va, va+filesz) with [pva, pva+PAGE_SIZE)
            unsigned long copy_start = max(pva, va);
            unsigned long copy_end   = min(pva + PAGE_SIZE, va + filesz);

            if (copy_start < copy_end) {
                unsigned long dst = pa + (copy_start - pva);
                unsigned long src = ph->offset + (copy_start - va);
                memcpy((void *)dst, data + src, copy_end - copy_start);
            }

            // Map the physical page into the user page table
            vm_map(pt, pva, pa, PAGE_SIZE, flags);
        }
    }

    return hdr->entry;
}
