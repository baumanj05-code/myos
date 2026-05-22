OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS {
    . = 0x10000;        /* user programs load here */
    .text  : { *(.text) *(.rodata) }
    .data  : { *(.data) }
    .bss   : { *(.bss)  }
}
