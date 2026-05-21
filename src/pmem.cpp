#include "pmem.h"
#include "print.h"

extern char _kernel_end[];  // defined by linker script

// Each free page stores a pointer to the next free page in its first 8 bytes
struct Page {
    Page *next;
};

static Page *freelist = nullptr;

void pmem_init() {
    unsigned long start = (unsigned long)_kernel_end;
    unsigned long end   = PHYS_END;
    unsigned long count = 0;

    for (unsigned long addr = start; addr + PAGE_SIZE <= end; addr += PAGE_SIZE) {
        pmem_free((void *)addr);
        count++;
    }

    kprintf("pmem: %d pages free (%dMB)\n",
            (int)count, (int)(count * PAGE_SIZE / (1024 * 1024)));
}

void *pmem_alloc() {
    if (!freelist) {
        kprintf("pmem: out of memory!\n");
        while (1) {}
    }
    Page *page = freelist;
    freelist = freelist->next;
    return page;
}

void pmem_free(void *addr) {
    Page *page = (Page *)addr;
    page->next = freelist;
    freelist = page;
}
