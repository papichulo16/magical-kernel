#pragma once
#include <stdint.h>

#define PAGE_SIZE 0x1000
#define NUM_PAGES 0x8000 * 8

#define RESB_KERN_SIZE 0x4000000 // 64MB
#define RESB_KERN_PAGES (RESB_KERN_SIZE / PAGE_SIZE)

typedef struct {
    uint8_t* bitmap;       
    uint64_t total;  
} mk_page_alloc_t;

void mk_page_alloc_init();

uint8_t* mk_phys_page_alloc();
void mk_phys_page_free(uint8_t* p);
