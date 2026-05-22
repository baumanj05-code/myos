#include "proc.h"
#include "pmem.h"
#include "vm.h"
#include "elf.h"
#include "print.h"

extern "C" void switch_context(Context *old, Context *new_ctx);

static Process procs[MAX_PROCS];
static int     next_pid = 1;
static Context sched_ctx;
       Process *current = nullptr;

void proc_init() {
    for (int i = 0; i < MAX_PROCS; i++) {
        procs[i].state = ProcState::UNUSED;
        procs[i].pid   = 0;
        procs[i].pt    = nullptr;
    }
}

Process *proc_create(void (*func)()) {
    for (int i = 0; i < MAX_PROCS; i++) {
        if (procs[i].state != ProcState::UNUSED) continue;

        Process *p  = &procs[i];
        p->pid      = next_pid++;
        p->kstack   = (char *)pmem_alloc();
        p->pt       = nullptr;
        p->ctx      = {};
        p->ctx.ra   = (unsigned long)func;
        p->ctx.sp   = (unsigned long)(p->kstack + KSTACK_SIZE);
        p->state    = ProcState::RUNNABLE;

        kprintf("proc: created kernel thread pid %d\n", p->pid);
        return p;
    }
    kprintf("proc: no free slots!\n");
    return nullptr;
}

// Trampoline called by the scheduler the first time a user process runs.
// Switches to the user page table and srets into U-mode.
static void user_trap_return() {
    Process *p = current;

    // Set sscratch = kernel stack top so trap_entry can switch stacks
    unsigned long kstack_top = (unsigned long)(p->kstack + KSTACK_SIZE);

    // Switch to user page table, set up CSRs, drop to U-mode
    asm volatile(
        "csrw sscratch, %0\n"   // kernel stack top for trap_entry
        "csrw sepc,     %1\n"   // entry point — where sret jumps
        "li   t0,       (1 << 18)\n" // SUM=1: S-mode can read user pages (needed for syscall bufs)
        "csrw sstatus,  t0\n"        // SPP=0 (U-mode), SUM=1
        "mv   sp,       %2\n"   // user stack pointer
        "csrw satp,     %3\n"   // switch to user page table
        "sfence.vma zero, zero\n"
        "sret\n"
        :: "r"(kstack_top),
           "r"(p->entry),
           "r"(p->ustack),
           "r"((8UL << 60) | ((unsigned long)p->pt >> 12))
    );
}

Process *proc_create_user(const unsigned char *elf_data) {
    for (int i = 0; i < MAX_PROCS; i++) {
        if (procs[i].state != ProcState::UNUSED) continue;

        Process *p = &procs[i];
        p->pid     = next_pid++;
        p->kstack  = (char *)pmem_alloc();

        // Create user page table (kernel mapped in upper half)
        p->pt = vm_create_user_pt();

        // Load ELF into user page table
        p->entry = elf_load(elf_data, p->pt);
        if (!p->entry) {
            kprintf("proc: elf_load failed\n");
            return nullptr;
        }

        // Map user stack just below USER_STACK_TOP
        void *stack_page = pmem_alloc();
        vm_map(p->pt, USER_STACK_TOP - PAGE_SIZE,
               (unsigned long)stack_page, PAGE_SIZE, PTE_R | PTE_W | PTE_U);
        p->ustack = USER_STACK_TOP;

        // Scheduler jumps to user_trap_return on first run
        p->ctx    = {};
        p->ctx.ra = (unsigned long)user_trap_return;
        p->ctx.sp = (unsigned long)(p->kstack + KSTACK_SIZE);
        p->state  = ProcState::RUNNABLE;

        kprintf("proc: created user process pid %d entry=0x%x\n",
                p->pid, (unsigned int)p->entry);
        return p;
    }
    kprintf("proc: no free slots!\n");
    return nullptr;
}

void yield() {
    Process *p = current;
    p->state   = ProcState::RUNNABLE;
    switch_context(&p->ctx, &sched_ctx);
}

void schedule() {
    while (1) {
        for (int i = 0; i < MAX_PROCS; i++) {
            if (procs[i].state != ProcState::RUNNABLE) continue;
            current        = &procs[i];
            current->state = ProcState::RUNNING;
            switch_context(&sched_ctx, &current->ctx);
        }
    }
}

void proc_exit(int code) {
    Process *p = current;
    kprintf("proc: pid %d exiting with code %d\n", p->pid, code);

    if (p->pt) {
        vm_free_user_pt(p->pt);
        p->pt = nullptr;
    }

    pmem_free(p->kstack);

    p->state = ProcState::UNUSED;
    switch_context(&p->ctx, &sched_ctx);
}