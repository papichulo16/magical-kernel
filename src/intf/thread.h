#pragma once

#include <stdint.h>
#include "context.h"

#define MAX_THREADS 0x10


enum ThreadState {
    MK_THREAD_KILLED,
    MK_THREAD_READY,
    MK_THREAD_RUNNING,
    MK_THREAD_BLOCKED,
    MK_THREAD_SLEEPING,
    MK_THREAD_ZOMBIE
};

struct mk_thread_obj {
    enum ThreadState state;
    struct regs_context regs;
    uint8_t started;

    void* entry;

    uint8_t* stack_base;
    uint8_t* stack_pos;
};


void mk_thread_create(void* entry);
void mk_thread_kill();
