#pragma once
#include <stdint.h>

#define PAGE_SIZE 0x1000

#define TOTAL_RESB_SIZE 0x100000000 // 4GB
#define NUM_PAGES (TOTAL_RESB_SIZE / PAGE_SIZE)

#define RESB_KERN_SIZE 0x4000000 // 64MB
#define RESB_KERN_PAGES (RESB_KERN_SIZE / PAGE_SIZE)

#define RESB_RAM_SIZE 0x8000000
#define RESB_RAM_PAGES (RESB_RAM_SIZE / PAGE_SIZE)

typedef struct {
    uint8_t* bitmap;       
    uint64_t total;  
} mk_page_alloc_t;

void mk_page_alloc_init();

uint8_t* mk_phys_page_alloc();
void mk_phys_page_free(uint8_t* p);
