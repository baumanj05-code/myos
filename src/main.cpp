#include "print.h"
#include "trap.h"
#include "pmem.h"
#include "vm.h"
#include "proc.h"
#include "user_program.h"

void main() {
    trap_init();
    pmem_init();
    vm_init();
    proc_init();

    proc_create_user(user_program);

    schedule();
}
