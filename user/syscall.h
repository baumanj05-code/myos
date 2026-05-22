#ifndef SYSCALL_H
#define SYSCALL_H

#define SYS_write  0
#define SYS_exit   1
#define SYS_getpid 2

long write(int fd, const char *buf, long len);
void exit(int code);
int  getpid();

#endif
