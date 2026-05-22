.section .text
.global trap_entry
.align 4

# sscratch convention:
#   0         = currently in kernel (S-mode)
#   non-zero  = currently in user (U-mode), holds kernel stack top

trap_entry:
    # Atomically swap sp with sscratch.
    # After swap: sp = old sscratch, sscratch = old sp
    csrrw sp, sscratch, sp
    bnez sp, from_user

from_kernel:
    # sscratch was 0 → came from kernel.
    # sp = 0 (bad), sscratch = original kernel sp. Swap back.
    csrrw sp, sscratch, sp      # sp = kernel sp, sscratch = 0
    addi sp, sp, -(33 * 8)
    # save x5 (t0) before using it
    sd x5,   5*8(sp)
    # save x2 (sp): original = current sp + frame size
    addi t0, sp, 33*8
    sd t0,   2*8(sp)
    j save_regs

from_user:
    # sscratch was kernel stack top → came from user.
    # sp = kernel stack top, sscratch = user sp
    addi sp, sp, -(33 * 8)
    # save x5 (t0) before using it
    sd x5,   5*8(sp)
    # save x2 (sp): the user sp is in sscratch
    csrr t0, sscratch
    sd t0,   2*8(sp)
    # clear sscratch — we're in kernel mode now
    csrw sscratch, zero

save_regs:
    sd x0,   0*8(sp)
    sd x1,   1*8(sp)
    # x2 and x5 already saved
    sd x3,   3*8(sp)
    sd x4,   4*8(sp)
    sd x6,   6*8(sp)
    sd x7,   7*8(sp)
    sd x8,   8*8(sp)
    sd x9,   9*8(sp)
    sd x10, 10*8(sp)
    sd x11, 11*8(sp)
    sd x12, 12*8(sp)
    sd x13, 13*8(sp)
    sd x14, 14*8(sp)
    sd x15, 15*8(sp)
    sd x16, 16*8(sp)
    sd x17, 17*8(sp)
    sd x18, 18*8(sp)
    sd x19, 19*8(sp)
    sd x20, 20*8(sp)
    sd x21, 21*8(sp)
    sd x22, 22*8(sp)
    sd x23, 23*8(sp)
    sd x24, 24*8(sp)
    sd x25, 25*8(sp)
    sd x26, 26*8(sp)
    sd x27, 27*8(sp)
    sd x28, 28*8(sp)
    sd x29, 29*8(sp)
    sd x30, 30*8(sp)
    sd x31, 31*8(sp)

    csrr t0, sepc
    sd t0, 32*8(sp)

    mv a0, sp
    call trap_handler

    ld t0, 32*8(sp)
    csrw sepc, t0

    # Restore all registers except sp and t0 (x5) — handled below
    ld x1,   1*8(sp)
    ld x3,   3*8(sp)
    ld x4,   4*8(sp)
    ld x6,   6*8(sp)
    ld x7,   7*8(sp)
    ld x8,   8*8(sp)
    ld x9,   9*8(sp)
    ld x10, 10*8(sp)
    ld x11, 11*8(sp)
    ld x12, 12*8(sp)
    ld x13, 13*8(sp)
    ld x14, 14*8(sp)
    ld x15, 15*8(sp)
    ld x16, 16*8(sp)
    ld x17, 17*8(sp)
    ld x18, 18*8(sp)
    ld x19, 19*8(sp)
    ld x20, 20*8(sp)
    ld x21, 21*8(sp)
    ld x22, 22*8(sp)
    ld x23, 23*8(sp)
    ld x24, 24*8(sp)
    ld x25, 25*8(sp)
    ld x26, 26*8(sp)
    ld x27, 27*8(sp)
    ld x28, 28*8(sp)
    ld x29, 29*8(sp)
    ld x30, 30*8(sp)
    ld x31, 31*8(sp)

    # Check sstatus.SPP (bit 8): 1 = returning to S-mode, 0 = returning to U-mode
    csrr t0, sstatus
    andi t0, t0, (1 << 8)
    bnez t0, to_kernel

to_user:
    # Restore sscratch = kernel stack top (for the next trap from user)
    addi t0, sp, 33*8
    csrw sscratch, t0
    ld x5, 5*8(sp)           # restore t0
    ld sp, 2*8(sp)           # restore user sp — must be last use of frame
    sret

to_kernel:
    ld x5,  5*8(sp)          # restore t0
    addi sp, sp, 33*8        # reclaim frame
    sret
