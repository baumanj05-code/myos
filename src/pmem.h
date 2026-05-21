#ifndef PMEM_H
#define PMEM_H

constexpr unsigned long PAGE_SIZE = 4096;
constexpr unsigned long PHYS_END  = 0x88000000;  // 128MB mark

void  pmem_init();
void *pmem_alloc();
void  pmem_free(void *addr);

#endif
