#ifndef SYSCALL_H
#define SYSCALL_H

#include "trap.h"

#define SYS_write  0
#define SYS_exit   1
#define SYS_getpid 2

void handle_syscall(TrapFrame *frame);

#endif
