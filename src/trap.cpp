#include "trap.h"
#include "print.h"
#include "syscall.h"

extern "C" void trap_entry();

static const char *exception_names[] = {
    "Instruction address misaligned",  // 0
    "Instruction access fault",        // 1
    "Illegal instruction",             // 2
    "Breakpoint",                      // 3
    "Load address misaligned",         // 4
    "Load access fault",               // 5
    "Store/AMO address misaligned",    // 6
    "Store/AMO access fault",          // 7
    "Environment call from U-mode",    // 8
    "Environment call from S-mode",    // 9
    nullptr,                           // 10 (reserved)
    "Environment call from M-mode",    // 11
    "Instruction page fault",          // 12
    "Load page fault",                 // 13
    nullptr,                           // 14 (reserved)
    "Store/AMO page fault",            // 15
};

static const char *interrupt_names[] = {
    nullptr, nullptr, nullptr,
    "Supervisor software interrupt",   // 3
    nullptr, nullptr, nullptr,
    "Supervisor timer interrupt",      // 7
    nullptr, nullptr, nullptr,
    "Supervisor external interrupt",   // 11
};

extern "C" void trap_handler(TrapFrame *frame) {
    unsigned long scause, stval;
    asm volatile("csrr %0, scause" : "=r"(scause));
    asm volatile("csrr %0, stval"  : "=r"(stval));

    bool is_interrupt = scause >> 63;
    unsigned long code = scause & ~(1UL << 63);

    if (!is_interrupt && code == 8) {
        // U-mode ecall — handle syscall and return
        handle_syscall(frame);
        frame->pc += 4;     // advance past ecall instruction
        return;
    }

    // All other traps: print info and hang
    kprintf("\n--- TRAP ---\n");
    if (is_interrupt) {
        const char *name = (code < 12 && interrupt_names[code])
                         ? interrupt_names[code] : "Unknown interrupt";
        kprintf("Interrupt: %s (code %d)\n", name, (int)code);
    } else {
        const char *name = (code < 16 && exception_names[code])
                         ? exception_names[code] : "Unknown exception";
        kprintf("Exception: %s (code %d)\n", name, (int)code);
    }
    kprintf("sepc:  0x%x\n", (unsigned int)frame->pc);
    kprintf("stval: 0x%x\n", (unsigned int)stval);

    while (1) {}
}

extern "C" void trap_init() {
    unsigned long addr = (unsigned long)trap_entry;
    asm volatile("csrw stvec, %0" :: "r"(addr));
}
