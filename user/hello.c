#include "syscall.h"

int main() {
    int pid = getpid();
    write(1, "Hello from user space!\n", 23);
    exit(0);
    return 0;
}
