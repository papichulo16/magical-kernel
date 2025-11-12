#include "page.h"

mk_page_alloc_t phys_allocator;
uint8_t b[0x10000];

static inline void set_bit(uint64_t bit) {
    phys_allocator.bitmap[bit / 8] |=  (1 << (bit % 8));
}

static inline void clear_bit(uint64_t bit) {
    phys_allocator.bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static inline int test_bit(uint64_t bit) {
    return phys_allocator.bitmap[bit / 8] & (1 << (bit % 8));
}
