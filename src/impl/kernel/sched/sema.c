#include "slab.h"
#include "thread.h"

void mk_create_sema(void** dst, int start) {
    struct mk_sema_t* p = mkmalloc(sizeof(struct mk_sema_t));

    p->state = start;
    p->head = 0;
    p->tail = 0;

    *dst = p;
}

void mk_sema_give(struct mk_sema_t* sema) {
    sema->state++;
    
    sema->head->state = MK_THREAD_READY;
    sema->head = sema->head->sema_next;
    
    if (sema->head == 0)
        sema->tail = 0;
}

void mk_sema_take(struct mk_sema_t* sema) {
    struct mk_thread_obj* t = mk_get_working_thread();
    
    t->state = MK_THREAD_SLEEPING;
    t->sema_next = 0;

    if (!sema->head)
        sema->head = t;
    else
        sema->tail->sema_next = t;

    sema->tail = t; 
    
    while (sema->state < 0);
    
    t->state = MK_THREAD_WORKING;
    sema->state--;
}
