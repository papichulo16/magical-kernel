#include "thread.h"
#include "page_temp.h"

// this setup is temporary, once I have a kmalloc() implementation
// I will make this better
struct mk_thread_obj threads[MAX_THREADS];
uint8_t thread_pos = 0;

void mk_thread_create(void* entry) {
    int t_idx = -1;

    for (int i = 0; i < MAX_THREADS; i++) {
        if (threads[i].state == MK_THREAD_KILLED) {
            t_idx = i;
        }
    }
    
    if (t_idx == -1)
        return;
    
    threads[t_idx].started = 0;
    threads[t_idx].entry = entry;
    threads[t_idx].stack_base = mk_temp_stack_alloc();
    threads[t_idx].stack_pos = threads[t_idx].stack_base;
}
