#include "thread.h"
#include "page_temp.h"
#include "print.h"
#include <stdint.h>
#include "inlines.c"

// this setup is temporary, once I have a kmalloc() implementation
// I will make this better
struct mk_thread_obj threads[MAX_THREADS];
uint8_t thread_pos = 0;

void mk_thread_create(void* entry) {
    int t_idx = -1;
    
    for (int i = 0; i < MAX_THREADS; i++) {
        if (threads[i].state == MK_THREAD_KILLED) {
            t_idx = i;
            
            break;
        }
    }
    
    if (t_idx == -1) {
        print_error("[!] Ran out of threads\n");

        return;
    }
    
    threads[t_idx].state = MK_THREAD_READY;
    threads[t_idx].started = 0;
    threads[t_idx].entry = entry;
    threads[t_idx].stack_base = mk_temp_page_alloc();

    threads[t_idx].regs.rsp = (uint64_t) threads[t_idx].stack_base + PAGE_SIZE;
    threads[t_idx].regs.rip = (uint64_t) threads[t_idx].entry;

    // temporary
    threads[t_idx].regs.cs = (uint64_t) 0x8;
    threads[t_idx].regs.ss = 0x10;
    threads[t_idx].regs.rflags = (uint64_t) 0x202;
    
}

void mk_thread_idle() {
    while(1);
}

void mk_thread_kill() {
    disable_interrupts();
    
    threads[thread_pos].state = MK_THREAD_KILLED;
    threads[thread_pos].time_slice = 0;
    threads[thread_pos].regs.rip = (uint64_t) &mk_thread_idle;
    
    mk_temp_page_free(threads[thread_pos].stack_base);
    
    enable_interrupts();
    
    mk_thread_idle();
}

int mk_thread_ctx_switch() {
    for (int i = (thread_pos + 1) % MAX_THREADS ; 
        i != thread_pos ; i = (i + 1) % MAX_THREADS) {
        
        if (threads[i].state == MK_THREAD_READY) {
            threads[thread_pos].state = MK_THREAD_READY;

            thread_pos = i;
            threads[thread_pos].state = MK_THREAD_WORKING;
            threads[thread_pos].time_slice = 10;
            
            return 0;
        }
    }
    
    if (threads[thread_pos].state == MK_THREAD_WORKING)
        return 0;

    // no available threads
    return 1;
}

struct mk_thread_obj* mk_get_working_thread() {
    return &threads[thread_pos];
}
