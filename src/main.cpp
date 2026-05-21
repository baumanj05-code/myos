#include "print.h"
#include "trap.h"
#include "pmem.h"
#include "vm.h"

void main() {
    trap_init();   // set stvec
    pmem_init();   // build free page list
    vm_init();     // build kernel page table, enable Sv39

    kprintf("kernel ready\n");
    while (1) {}
}
