#include "slab.h"
#include "thread.h"
#include "print.h"
#include "inlines.c"

#define SEMA_DEBUG 0

void mk_create_sema(void** dst, int start) {
    struct mk_sema_t* p = mkmalloc(sizeof(struct mk_sema_t));

    p->state = start;
    p->head = 0;
    p->tail = 0;

    *dst = p;
}

void mk_sema_give(struct mk_sema_t* sema) {
    sema->state++;
    
    struct mk_thread_obj* t = sema->head;
    
    if (t == 0)
        return;

    t->state = MK_THREAD_READY;
    sema->head = t->sema_next;

    if (SEMA_DEBUG) {
        print_str("[*] sema.c: woken thread ");
        print_str(t->thread_name);
        print_char('\n');
    }
    
    if (sema->head == 0)
        sema->tail = 0;
}

void mk_sema_take(struct mk_sema_t* sema) {
    disable_interrupts();

    struct mk_thread_obj* t = mk_get_working_thread();
    
    t->state = MK_THREAD_SLEEPING;
    t->sema_next = 0;

    if (!sema->head)
        sema->head = t;
    else
        sema->tail->sema_next = t;

    sema->tail = t; 
    
    if (SEMA_DEBUG) {
        print_str("[*] sema.c: thread ");
        print_str(t->thread_name);
        print_str(" sleeping\n");
    }
    
    enable_interrupts();

    while (sema->state < 0);// && t->state == MK_THREAD_SLEEPING);
    
    if (SEMA_DEBUG) {
        print_str("[*] sema.c: thread ");
        print_str(t->thread_name);
        print_str(" continued\n");
    }

    t->state = MK_THREAD_WORKING;
    sema->state--;
}
