#include "proc.h"
#include "pmem.h"
#include "print.h"

extern "C" void switch_context(Context *old, Context *new_ctx);

static Process procs[MAX_PROCS];
static int     next_pid    = 1;
static Context sched_ctx;           // scheduler's own saved context
       Process *current    = nullptr;

void proc_init() {
    for (int i = 0; i < MAX_PROCS; i++) {
        procs[i].state = ProcState::UNUSED;
        procs[i].pid   = 0;
    }
}

Process *proc_create(void (*func)()) {
    // find a free slot
    for (int i = 0; i < MAX_PROCS; i++) {
        if (procs[i].state != ProcState::UNUSED) continue;

        Process *p = &procs[i];
        p->pid     = next_pid++;
        p->kstack  = (char *)pmem_alloc();

        // set up initial context:
        // ra = func  → ret in switch_context jumps here on first run
        // sp = top of kernel stack
        p->ctx     = {};
        p->ctx.ra  = (unsigned long)func;
        p->ctx.sp  = (unsigned long)(p->kstack + KSTACK_SIZE);
        p->state   = ProcState::RUNNABLE;

        kprintf("proc: created pid %d\n", p->pid);
        return p;
    }

    kprintf("proc: no free slots!\n");
    return nullptr;
}

// Give up the CPU — switch back to the scheduler
void yield() {
    Process *p = current;
    p->state   = ProcState::RUNNABLE;
    switch_context(&p->ctx, &sched_ctx);
    // scheduler will eventually switch back here, resuming after this line
}

// Round-robin scheduler — never returns
void schedule() {
    while (1) {
        for (int i = 0; i < MAX_PROCS; i++) {
            if (procs[i].state != ProcState::RUNNABLE) continue;

            current        = &procs[i];
            current->state = ProcState::RUNNING;
            switch_context(&sched_ctx, &current->ctx);
            // process yielded — loop continues to find next
        }
    }
}
