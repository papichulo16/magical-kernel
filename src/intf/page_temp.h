#pragma once

#include <stdint.h>

#define NUM_PAGES 0x100
#define PAGE_SIZE 0x2000

void mk_temp_page_init();

uint8_t* mk_temp_page_alloc();
uint8_t* mk_temp_stack_alloc();

void mk_temp_page_free(uint8_t* p);
void mk_temp_stack_free(uint8_t* p);

struct mk_free_metadata {
    uint8_t is_free;

    struct mk_free_metadata* next;
};

union mk_temp_page {
    struct mk_free_metadata freed;

    uint8_t data[PAGE_SIZE];
};

