#include "syscall.h"
#include "print.h"
#include "uart.h"
#include "proc.h"

static long sys_write(int fd, const char *buf, long len) {
    // buf is a user virtual address — readable because SUM=1 in sstatus
    (void)fd;   // only stdout for now
    for (long i = 0; i < len; i++)
        uart_putc(buf[i]);
    return len;
}

static void sys_exit(int code) {
    proc_exit(code);
    // proc_exit never returns (switches to scheduler)
}

static int sys_getpid() {
    return current->pid;
}

void handle_syscall(TrapFrame *frame) {
    unsigned long num = frame->regs[17];  // a7 = syscall number
    unsigned long a0  = frame->regs[10];
    unsigned long a1  = frame->regs[11];
    unsigned long a2  = frame->regs[12];

    switch (num) {
        case SYS_write:
            frame->regs[10] = sys_write((int)a0, (const char *)a1, (long)a2);
            break;
        case SYS_exit:
            sys_exit((int)a0);
            break;  // never reached
        case SYS_getpid:
            frame->regs[10] = sys_getpid();
            break;
        default:
            kprintf("syscall: unknown number %d\n", (int)num);
            frame->regs[10] = -1;
            break;
    }
}
