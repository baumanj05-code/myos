#include "print.h"

void main() {
    kprintf("Hello from RISC-V!\n");
    kprintf("decimal: %d\n", 42);
    kprintf("hex:     0x%x\n", 0xdeadbeef);
    kprintf("char:    %c\n", 'B');
    kprintf("string:  %s\n", "it works");
    while (1) {}
}
