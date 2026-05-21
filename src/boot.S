.section .text
.global _start
_start:
    la sp, stack_top       # set up stack pointer
    call __init_cpp        # call global constructors

    # Grant S-mode access to all physical memory via PMP.
    # pmpaddr0 = all 1s, pmpcfg0 = TOR mode + RWX (0x0f)
    li t0, -1
    csrw pmpaddr0, t0
    li t0, 0x0f
    csrw pmpcfg0, t0

    # Delegate all exceptions and interrupts to S-mode
    li t0, 0xffff
    csrw medeleg, t0
    csrw mideleg, t0

    # Set mstatus.MPP = 01 (S-mode) so mret drops to S-mode
    li t0, (1 << 11)
    csrw mstatus, t0

    # mret will jump to main in S-mode
    la t0, main
    csrw mepc, t0
    mret

halt:
    j halt

.section .bss
.align 4
stack_bottom:
    .space 4096
stack_top:
