#pragma once

#include <stdint.h>

void mk_thread_create(void* entry);
void mk_thread_restore();
void mk_context_switch();
void mk_thread_kill();

struct mk_thread_obj {
    void* entry;

    uint8_t* stack_base;
    uint8_t* stack_pos;
};

