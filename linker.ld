OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS {
    . = 0x80000000;     /* QEMU virt machine loads here */
    .text : { *(.text) *(.rodata) }
    .init_array : {
        __init_array_start = .;
        *(.init_array)
        __init_array_end = .;
    }
    .data : { *(.data) }
    .bss  : { *(.bss)  }
}
