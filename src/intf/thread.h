#pragma once

#include <stdint.h>
#include "context.h"

#define MAX_THREAD_PRIO 0x10
#define THREAD_TIME_SLICE 10

enum ThreadState {
    MK_THREAD_KILLED,
    MK_THREAD_READY,
    MK_THREAD_WORKING,
    MK_THREAD_BLOCKED,
    MK_THREAD_SLEEPING,
    MK_THREAD_ZOMBIE
};

/*
struct mk_thread_obj {
    enum ThreadState state;
    struct regs_context regs;

    uint8_t started;
    uint32_t time_slice;

    void* entry;

    uint8_t* stack_base;
};
*/

struct mk_thread_obj {
    char* thread_name;

    enum ThreadState state;
    struct regs_context regs;

    uint8_t started;
    // uint8_t prio;
    uint32_t time_slice;

    void* entry;

    uint8_t* stack_base;

    struct mk_thread_obj* next;
    struct mk_thread_obj* prev;
    
    // struct mk_thread_obj* sema_next;
};

struct mk_sema_t {
    int state;

    struct mk_thread_obj* head;
    struct mk_thread_obj* tail;
};

void mk_thread_create(void* entry, char* thread_name);
void mk_thread_kill();

int mk_thread_ctx_switch();
struct mk_thread_obj* mk_get_working_thread();

void mk_t_arr_enq(struct mk_thread_obj* t);
void mk_t_arr_deq(int free);

void mk_create_sema(void** dst, int start);
void mk_sema_give(struct mk_sema_t* sema);
void mk_sema_take(struct mk_sema_t* sema);

void mk_sema_enq(struct mk_sema_t* sema, struct mk_thread_obj* t);
void mk_sema_deq(struct mk_sema_t* sema, struct mk_thread_obj* t);

void mk_sema_sig_set(struct mk_sema_t* sema);

