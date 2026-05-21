#ifndef VM_H
#define VM_H

// Sv39 page table entry flags
constexpr unsigned long PTE_V = 1 << 0;  // Valid
constexpr unsigned long PTE_R = 1 << 1;  // Readable
constexpr unsigned long PTE_W = 1 << 2;  // Writable
constexpr unsigned long PTE_X = 1 << 3;  // Executable
constexpr unsigned long PTE_U = 1 << 4;  // User accessible

using pte_t       = unsigned long;
using pagetable_t = pte_t *;        // 512 PTEs per page table

void vm_init();
int  vm_map(pagetable_t pt, unsigned long va, unsigned long pa,
            unsigned long size, unsigned long flags);

#endif
