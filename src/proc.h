#ifndef PROC_H
#define PROC_H

#include "vm.h"

enum class ProcState { UNUSED, RUNNABLE, RUNNING };

// Callee-saved registers — all we need to save on a context switch
struct Context {
    unsigned long ra;
    unsigned long sp;
    unsigned long s0,  s1,  s2,  s3,  s4,  s5,
                  s6,  s7,  s8,  s9,  s10, s11;
};

constexpr int          KSTACK_SIZE    = 4096;
constexpr int          MAX_PROCS      = 8;
constexpr unsigned long USER_STACK_TOP = 0x7fff0000;

struct Process {
    int            pid;
    ProcState      state;
    Context        ctx;
    char          *kstack;   // bottom of kernel stack
    pagetable_t    pt;       // user page table (nullptr for kernel threads)
    unsigned long  entry;    // user entry point
    unsigned long  ustack;   // user stack pointer
};

extern Process *current;

void     proc_init();
Process *proc_create(void (*func)());
Process *proc_create_user(const unsigned char *elf_data);
void     yield();
void     schedule();
void     proc_exit(int code);

#endif
