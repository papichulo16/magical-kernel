#include "print.h"
#include "thread.h"
#include "page.h"
#include "inlines.c"
#include "virt.h"
#include "slab.h"
#include <stdint.h>

#define THREAD_DEBUG 0

struct mk_thread_obj* prio_thread_arr[MAX_THREAD_PRIO];
struct mk_thread_obj* working_thread = 0;

struct mk_thread_obj* mk_get_working_thread() {
    return working_thread;
}

void mk_thread_idle() {
    while(1);
}

void thread_init_basic(struct mk_thread_obj* t, void* entry, char* thread_name) {
    t->thread_name = thread_name;
    
    t->state = MK_THREAD_READY;
    t->started = 0;
    t->entry = entry;
    t->stack_base = mk_vmmap_l1(0);
    t->time_slice = 0;
    
    t->regs.rsp = (uint64_t) t->stack_base + PAGE_SIZE;
    t->regs.rip = (uint64_t) t->entry;

    // temporary
    t->regs.cs = (uint64_t) 0x8;
    t->regs.ss = 0x10;
    t->regs.rflags = (uint64_t) 0x202;   
}

void thread_arr_push(struct mk_thread_obj* t, uint8_t prio) {
    t->prio = prio;
    t->next = prio_thread_arr[prio];
    prio_thread_arr[prio] = t;
    
    if (t->next == 0) {
        t->next = t;
        t->prev = t;
    } else {
        t->prev = t->next->prev;
        t->next->prev = t;
    }
    
    if (working_thread == 0)
        working_thread = t;
}

void mk_thread_create(void* entry, uint8_t prio, char* thread_name) {
    if (prio >= MAX_THREAD_PRIO)
        return;
    
    struct mk_thread_obj* t = mkmalloc(sizeof(struct mk_thread_obj));

    thread_init_basic(t, entry, thread_name);
    thread_arr_push(t, prio);
}

void mk_thread_kill() {
    disable_interrupts();

    if (working_thread->next == working_thread) {
        prio_thread_arr[working_thread->prio] = 0;
    } else {
        working_thread->next->prev = working_thread->prev;
        working_thread->prev->next = working_thread->next;
    }
    
    if (THREAD_DEBUG) {
        print_str("[*] rtos_thread.c: thread ");
        print_str(working_thread->thread_name);
        print_str(" done\n");
    }
    
    working_thread->state = MK_THREAD_KILLED;
    working_thread->time_slice = 0;
    working_thread->regs.rip = (uint64_t) &mk_thread_idle;

    enable_interrupts();
    
    mk_thread_idle();
}

int mk_thread_ctx_switch() {

    if (working_thread->state == MK_THREAD_KILLED)
        working_thread = 0;

    for (int i = 0; i < MAX_THREAD_PRIO; i++) {
        struct mk_thread_obj* cur = prio_thread_arr[i];
        
        if (cur == 0)
            continue;
        
        do {
            
            if (cur->state == MK_THREAD_READY) {
                
                if (THREAD_DEBUG) {
                    print_str("[*] rtos_thread.c: thread ");
                    print_str(cur->thread_name);
                    print_str(" ready\n");
                }

                prio_thread_arr[i] = cur->next;
                
                working_thread->state = MK_THREAD_READY;
                cur->state = MK_THREAD_WORKING;
                cur->time_slice = THREAD_TIME_SLICE;

                working_thread = cur;
                
                return 0;
            }

            cur = cur->next;

        } while (cur != prio_thread_arr[i]);

    }

    return 1;
}
