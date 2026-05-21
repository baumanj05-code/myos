#include "print.h"
#include "trap.h"
#include "pmem.h"
#include "vm.h"
#include "proc.h"

void proc_a() {
    int i = 0;
    while (1) {
        if(i%1000000 == 0) kprintf("A: tick %d\n", i);
        i++;
        yield();
    }
}

void proc_b() {
    int i = 0;
    while (1) {
        if(i%1000000 == 0) kprintf("B: tick %d\n", i);
        i++;
        yield();
    }
}

void main() {
    trap_init();
    pmem_init();
    vm_init();
    proc_init();

    proc_create(proc_a);
    proc_create(proc_b);

    schedule();   // never returns
}
