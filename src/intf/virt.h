#pragma once

#include <stdint.h>

// defined in linker
extern uint8_t KERNEL_VIRT_START[];
extern uint8_t KERNEL_VIRT_END[];
extern uint8_t KERNEL_VMA[];

extern uint64_t KERNEL_SIZE;
extern uint64_t KERNEL_TABLES_VMA;
extern uint64_t KERNEL_DATA_VMA;

uint64_t mk_phys_to_mmio_virt(uint8_t* paddr);

void* mk_g_paddr(void* vaddr);
void* _mk_g_paddr(void* vaddr);

uint8_t* mk_vmmap_l1(uint8_t flags);
// uint8_t* mk_vmmap_l2(uint8_t flags);

void mk_unmmap_l1(uint8_t* vaddr);
// void mk_unmmap_l2(uint8_t* vaddr);

void mk_virt_init();
