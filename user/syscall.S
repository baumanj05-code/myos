.section .text

# Arguments already in a0-a2 per calling convention, just set a7 and ecall

.global write
write:
    li a7, 0        # SYS_write
    ecall
    ret

.global exit
exit:
    li a7, 1        # SYS_exit
    ecall
    ret

.global getpid
getpid:
    li a7, 2        # SYS_getpid
    ecall
    ret
