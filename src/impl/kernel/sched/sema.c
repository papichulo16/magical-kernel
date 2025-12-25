#include "slab.h"
#include "thread.h"
#include "print.h"
#include "inlines.c"

#define SEMA_DEBUG 0

void mk_sema_enq(struct mk_sema_t* sema, struct mk_thread_obj* t) {
    mk_t_arr_deq(0);

    t->next = 0;
    t->prev = 0;

    if (!sema->head) {
        sema->head = t;
    } else {
        sema->tail->next = t;
    }

    sema->tail = t; 
}

void mk_sema_deq(struct mk_sema_t* sema, struct mk_thread_obj* t) {
    sema->head = t->next;

    if (sema->head == 0)
        sema->tail = 0;

    mk_t_arr_enq(t);
}

void mk_create_sema(void** dst, int start) {
    struct mk_sema_t* p = mkmalloc(sizeof(struct mk_sema_t));

    p->state = start;
    p->head = 0;
    p->tail = 0;

    *(struct mk_sema_t **) dst = p;
}

void mk_sema_give(struct mk_sema_t* sema) {
    sema->state++;
    
    struct mk_thread_obj* t = sema->head;
    
    if (t == 0)
        return;

    disable_interrupts();

    t->state = MK_THREAD_READY;
    mk_sema_deq(sema, t);

    enable_interrupts();

    if (SEMA_DEBUG) {
        print_str("[*] sema.c: woken thread ");
        print_str(t->thread_name);
        print_char('\n');
    }
}

void mk_sema_take(struct mk_sema_t* sema) {
    disable_interrupts();

    struct mk_thread_obj* t = mk_get_working_thread();
    
    if (sema->state < 1) {
	t->state = MK_THREAD_SLEEPING;
	mk_sema_sig_set(sema);

    	if (SEMA_DEBUG) {
	    print_str("[*] sema.c: thread ");
	    print_str(t->thread_name);
	    print_str(" sleeping\n");
	}
    }

    sema->state--;
    t->time_slice = 0;
    
    enable_interrupts();

    while (t->state == MK_THREAD_SLEEPING);
    
    if (SEMA_DEBUG) {
        print_str("[*] sema.c: thread ");
        print_str(t->thread_name);
        print_str(" continued\n");
    }

    t->state = MK_THREAD_WORKING;
}
