#pragma once
#include <stdint.h>

#define PAGE_SIZE 0x1000

extern uint8_t KERNEL_PHYS_START[];
extern uint8_t KERNEL_PHYS_END[];

typedef struct {
    uint8_t* bitmap;       
    uint64_t total;  
} mk_page_alloc_t;

void mk_allocator_init();

uint8_t* mk_page_alloc();
uint8_t* mk_page_free(uint8_t* p);
