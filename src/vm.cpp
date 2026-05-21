#include "vm.h"
#include "pmem.h"
#include "print.h"

// PTE bit manipulation
// PA → PTE bits: (PA >> 12) << 10 == PA >> 2 for page-aligned addresses
#define PA_TO_PTE(pa)    (((unsigned long)(pa) >> 12) << 10)
#define PTE_TO_PA(pte)   (((pte) >> 10) << 12)

// Extract 9-bit VPN at a given level from a virtual address
#define VPN(va, level)   (((va) >> (12 + 9 * (level))) & 0x1FF)

// satp register: MODE=8 (Sv39), PPN of root page table
#define SATP_SV39        (8UL << 60)
#define MAKE_SATP(pt)    (SATP_SV39 | ((unsigned long)(pt) >> 12))

static pagetable_t kernel_pt;

// Walk 3 levels of page tables for va, allocating missing tables if alloc=true.
// Returns a pointer to the leaf PTE, or nullptr on failure.
static pte_t *vm_walk(pagetable_t pt, unsigned long va, bool alloc) {
    for (int level = 2; level > 0; level--) {
        pte_t *pte = &pt[VPN(va, level)];

        if (*pte & PTE_V) {
            // follow existing pointer to next-level table
            pt = (pagetable_t)PTE_TO_PA(*pte);
        } else {
            if (!alloc) return nullptr;
            // allocate and zero a new page table
            pt = (pagetable_t)pmem_alloc();
            for (int i = 0; i < 512; i++) pt[i] = 0;
            // non-leaf PTE: PTE_V only, no R/W/X
            *pte = PA_TO_PTE((unsigned long)pt) | PTE_V;
        }
    }
    return &pt[VPN(va, 0)];
}

// Map [va, va+size) → [pa, pa+size) with given flags.
// Both va and pa must be page-aligned. Returns 0 on success, -1 on failure.
int vm_map(pagetable_t pt, unsigned long va, unsigned long pa,
           unsigned long size, unsigned long flags) {
    for (unsigned long end = va + size; va < end; va += PAGE_SIZE, pa += PAGE_SIZE) {
        pte_t *pte = vm_walk(pt, va, true);
        if (!pte) return -1;
        *pte = PA_TO_PTE(pa) | flags | PTE_V;
    }
    return 0;
}

void vm_init() {
    // Allocate and zero the root page table
    kernel_pt = (pagetable_t)pmem_alloc();
    for (int i = 0; i < 512; i++) kernel_pt[i] = 0;

    // Identity map all physical RAM (kernel + free pages)
    vm_map(kernel_pt, 0x80000000, 0x80000000,
           PHYS_END - 0x80000000, PTE_R | PTE_W | PTE_X);

    // Identity map UART so kprintf keeps working
    vm_map(kernel_pt, 0x10000000, 0x10000000, PAGE_SIZE, PTE_R | PTE_W);

    // Enable Sv39: write satp and flush TLB
    asm volatile("csrw satp, %0" :: "r"(MAKE_SATP((unsigned long)kernel_pt)));
    asm volatile("sfence.vma zero, zero");

    kprintf("vm: Sv39 paging enabled\n");
}
