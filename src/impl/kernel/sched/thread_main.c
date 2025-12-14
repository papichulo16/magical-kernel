#include "print.h"
#include "thread.h"
#include "page.h"
#include "inlines.c"
#include "virt.h"
#include "slab.h"
#include <stdint.h>

#define THREAD_DEBUG 1

// this would be for priority based scheduling, right now the implementation will be R-R
// struct mk_thread_obj* prio_thread_arr[MAX_THREAD_PRIO];
struct mk_thread_obj* working_thread = 0;

struct mk_thread_obj* mk_get_working_thread() {
    return working_thread;
}

void mk_thread_idle() {
    while(1);
}

void t_init_basic(struct mk_thread_obj* t, void* entry, char* thread_name) {
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

void mk_t_arr_enq(struct mk_thread_obj* t) {

    if (working_thread == 0) {
        t->next = t;
        t->prev = t;

        working_thread = t;

	return;
    } 

    t->next = working_thread;
    t->prev = working_thread->prev;

    t->prev->next = t;
    working_thread->prev = t;

}

void mk_t_arr_deq(int free) {
    struct mk_thread_obj* cur = working_thread;

    if (cur->next != cur) {
        cur->next->prev = cur->prev;
        cur->prev->next = cur->next;
    }

    working_thread = working_thread->next;

    if (free) {

	if (cur->stack_base)
		mk_unmmap_l1(cur->stack_base);

	mkfree(cur);
    }
}

void mk_thread_create(void* entry, char* thread_name) {
    
    struct mk_thread_obj* t = mkmalloc(sizeof(struct mk_thread_obj));

    t_init_basic(t, entry, thread_name);
    mk_t_arr_enq(t);

}

void mk_thread_kill() {
    disable_interrupts();
    
    if (THREAD_DEBUG) {
        print_str("[*] thread_main.c: thread ");
        print_str(working_thread->thread_name);
        print_str(" done\n");
    }
    
    // thread obj is free'd in timer int handler
    working_thread->state = MK_THREAD_KILLED;
    working_thread->time_slice = 0;
    working_thread->regs.rip = (uint64_t) &mk_thread_idle;

    enable_interrupts();
    
    mk_thread_idle();
}

void t_switch(struct mk_thread_obj* cur) {

    if (THREAD_DEBUG) {
	print_str("[*] thread_main.c: thread ");
	print_str(cur->thread_name);
	print_str(" ready\n");
    }

    working_thread->state = MK_THREAD_READY;
    cur->state = MK_THREAD_WORKING;
    cur->time_slice = THREAD_TIME_SLICE;

    working_thread = cur;
}

int mk_thread_ctx_switch() {

    if (working_thread->state == MK_THREAD_KILLED) {
	mk_t_arr_deq(1);

	if (THREAD_DEBUG) {
	      print_str("[*] thread_main.c: ");
	      print_str(working_thread->thread_name);
	      print_str(" thread free'd\n");
	}

	disable_interrupts();
	mk_thread_idle();
    }

    struct mk_thread_obj* cur = working_thread;

    if (cur == 0)
	return 1;

    do {
	    
        if (cur->state == MK_THREAD_READY) {
		
	    t_switch(cur);
	
	    return 0;
	}

	cur = cur->next;

    } while (cur != working_thread);

    return 1;
}
