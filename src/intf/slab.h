#pragma once

#include <stdint.h>

// 8, 16, 32, 64, 128, 256, 512, 1024, 2048
#define NUM_BUCKETS 9

struct mk_slab_t {
    uint8_t id;
    uint32_t size;
    uint32_t free_count;
    uint32_t max;

    struct mk_slab_t* next;
    void* freelist_head;
};

struct mk_cache_node_t {
    struct mk_slab_t* buckets[NUM_BUCKETS];
};

void* mkmalloc (uint32_t s);
void mkfree (void* p);
