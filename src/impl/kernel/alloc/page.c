#include "page.h"
#include "print.h"
#include "isr.h"
#include <stdint.h>

mk_page_alloc_t phys_allocator;
uint8_t b[NUM_PAGES / 8];

static inline void set_bit(uint64_t bit) {
    phys_allocator.bitmap[bit / 8] |=  (1 << (bit % 8));
}

static inline void clear_bit(uint64_t bit) {
    phys_allocator.bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static inline int test_bit(uint64_t bit) {
    return phys_allocator.bitmap[bit / 8] & (1 << (bit % 8));
}

void mk_phys_page_free(uint8_t* p) {

    uint64_t i = ((uint64_t) p & ~0xfff) / PAGE_SIZE;

    if ((uint64_t) p % 0x1000 != 0) {
        print_error("[!] page.c: unaligned page free\n");
        halt();
    }
    
    if (i > NUM_PAGES) {
        print_error("[!] page.c: out of bounds page free\n");
        halt();
    }
    
    if (!test_bit(i)) {
        print_error("[!] page.c: page is already free\n");
        halt();
    }
    
    clear_bit(i);
}

uint8_t* mk_phys_page_alloc() {
    uint8_t* p = 0;
    
    for (uint64_t i = 0; i < NUM_PAGES && p == 0; i++) {
        if (test_bit(i))
            continue;
        
        p = (uint8_t *) (i * PAGE_SIZE);
        set_bit(i);
    }
    
    return p;
}

void mk_page_alloc_init() {
    phys_allocator.bitmap = b;
    phys_allocator.total = NUM_PAGES;
    
    // mark the already allocated memory from boot as used
    for (uint64_t i = 0; i < 0x2000; i++) {
        set_bit(i);
    }
}
