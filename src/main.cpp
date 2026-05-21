#include "print.h"

void main() {
    kprintf("Trap handler test\n");
    asm volatile("ebreak");  // trigger a breakpoint exception
    while (1) {}
}
