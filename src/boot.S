.section .text
.global _start
_start:
    la sp, stack_top       # set up stack pointer
    call __init_cpp        # call global constructors
    call trap_init         # set up mtvec
    call main
halt:
    j halt                 # loop forever

.section .bss
.align 4
stack_bottom:
    .space 4096
stack_top:
