.section .text
.global _start
_start:
    call main

    # fall through to exit(0) if main returns
    li a0, 0        # exit code 0
    li a7, 1        # SYS_exit = 1
    ecall

loop:
    j loop          # should never reach here
