#pragma once

#include <stdint.h>
#include "context.h"

#define MAX_THREADS 0x10


enum ThreadState {
    MK_THREAD_KILLED,
    MK_THREAD_READY,
    MK_THREAD_WORKING,
    MK_THREAD_BLOCKED,
    MK_THREAD_SLEEPING,
    MK_THREAD_ZOMBIE
};

struct mk_thread_obj {
    enum ThreadState state;
    struct regs_context regs;

    uint8_t started;
    uint32_t time_slice;

    void* entry;

    uint8_t* stack_base;
};

void _mk_thread_create(void* entry);
void _mk_thread_kill();

int _mk_thread_ctx_switch();

void mk_thread_create(void* entry);
void mk_thread_kill();

int mk_thread_ctx_switch();
struct mk_thread_obj* mk_get_working_thread();
