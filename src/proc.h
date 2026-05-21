#ifndef PROC_H
#define PROC_H

enum class ProcState { UNUSED, RUNNABLE, RUNNING };

// Callee-saved registers — all we need to save on a context switch
struct Context {
    unsigned long ra;
    unsigned long sp;
    unsigned long s0,  s1,  s2,  s3,  s4,  s5,
                  s6,  s7,  s8,  s9,  s10, s11;
};

constexpr int KSTACK_SIZE = 4096;
constexpr int MAX_PROCS   = 8;

struct Process {
    int       pid;
    ProcState state;
    Context   ctx;
    char     *kstack;   // bottom of this process's kernel stack
};

void     proc_init();
Process *proc_create(void (*func)());
void     yield();
void     schedule();

#endif
