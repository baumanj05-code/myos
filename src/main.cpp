#include "print.h"
#include "pmem.h"

void main() {
    pmem_init();

    void *p1 = pmem_alloc();
    void *p2 = pmem_alloc();
    kprintf("alloc 1: 0x%x\n", (unsigned int)(unsigned long)p1);
    kprintf("alloc 2: 0x%x\n", (unsigned int)(unsigned long)p2);

    pmem_free(p1);
    pmem_free(p2);
    kprintf("freed both pages\n");

    while (1) {}
}
