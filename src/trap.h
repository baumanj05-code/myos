#ifndef TRAP_H
#define TRAP_H

struct TrapFrame {
    unsigned long regs[32];  // x0-x31
    unsigned long pc;        // mepc
};

extern "C" void trap_init();

#endif
