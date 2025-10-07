#include "page_temp.h"
#include "isr.h"
#include "print.h"

// this file is just for a temporary page allocator so I
// can work on the scheduler
union mk_temp_page pages[NUM_PAGES];
struct mk_free_metadata* freelist_head = 0;

void mk_temp_page_init() {
    for (int i = 0; i < NUM_PAGES; i++) {
        pages[i].freed.is_free = 1;

        if (i < NUM_PAGES-1)
            pages[i].freed.next = (struct mk_free_metadata *) &pages[i+1];
        
        if (freelist_head == 0)
            freelist_head = (struct mk_free_metadata *) &pages[i];
    }
}

uint8_t* mk_temp_page_alloc() {
    struct mk_free_metadata* p = 0;

    if (freelist_head == 0)
        return 0;

    if (freelist_head->is_free == 0) {
        print_error("[!] Corruption in freelist\n");
        mk_exception_handler();
    }
    
    p = freelist_head;
    freelist_head = freelist_head->next;
    
    p->is_free = 0;
    
    return (uint8_t *) p;
}

uint8_t* mk_temp_stack_alloc() {
    uint8_t* p = mk_temp_page_alloc();
    
    if (p == 0)
        return 0;

    return p + PAGE_SIZE;
}

void mk_temp_page_free(uint8_t* p) {
    union mk_temp_page* ptr = (union mk_temp_page *) p;
    
    if (ptr < (union mk_temp_page *) &pages || 
        ptr > (union mk_temp_page *) (&pages + (NUM_PAGES * PAGE_SIZE))) {
        
        print_error("[!] Invalid free ptr\n");
        mk_exception_handler();
    }
    
    if ((ptr - (union mk_temp_page *) &pages) % PAGE_SIZE != 0) {
        print_error("[!] Misaligned page ptr\n");
        mk_exception_handler();
    }
    
    ptr->freed.is_free = 1;
    ptr->freed.next = freelist_head;

    freelist_head = (struct mk_free_metadata *) ptr;
}

void mk_temp_stack_free(uint8_t* p) {
    mk_temp_page_free(p - PAGE_SIZE);
}
