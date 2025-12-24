#include "slab.h"
#include "page.h"
#include "print.h"
#include "virt.h"
#include "inlines.c"

#include <stdint.h>

#define SLAB_DEBUGGING 0

struct mk_cache_node_t main_cache;

int get_bucket_id (uint32_t s) {
    uint32_t id = 0;
    s = s >> 3;
    
    while (s > 0) {
        s = s >> 1;
        id++;
    } 
    
    return id;
}

void new_slab_push (struct mk_cache_node_t* c, int id) {
    struct mk_slab_t* slab = (struct mk_slab_t *) mk_vmmap_l1(0);
    uint32_t s = (8 << id);
    uint32_t n = (int) ((PAGE_SIZE - sizeof(struct mk_slab_t)) / s);
    uint8_t** p = (uint8_t **) ((uint64_t) slab + sizeof(struct mk_slab_t));
    
    if (SLAB_DEBUGGING) {
        print_str("[*] slab.c: pushing new slab of id ");
        print_dword(id);
        print_str(" at vaddr ");
        print_qword((uint64_t) slab);
        print_char('\n');
    }
    
    slab->id = id;
    slab->size = s;
    slab->free_count = n;
    slab->max = n;
    slab->freelist_head = p;
    slab->next = c->buckets[id];
    
    for (int i = 0; i < n-1; i++, 
        p += (s / 8)) {
        
        *p = (uint8_t *) ((uint64_t) p + s);

    }

    c->buckets[id] = slab;
}

void* alloc_from_slab (struct mk_slab_t* slab) {
    void* p = 0;

    while (slab != 0 && p == 0) {

        if (slab->free_count < 1) {
            slab = slab->next;
            continue;
        }
        
        p = slab->freelist_head;
        slab->freelist_head = *(void **) p;
        slab->free_count--;

        if (SLAB_DEBUGGING) {
            print_str("[*] slab.c: alloc obj at vaddr ");
            print_qword((uint64_t) p);
            print_str(" from slab vaddr ");
            print_qword((uint64_t) slab);
            print_str(". ");
            print_dword((uint32_t) slab->free_count);
            print_str(" objs rem on slab\n");
        }
    }
    
    return p;
}

void reclaim_slab (struct mk_cache_node_t* c, struct mk_slab_t* s) {
    int id = s->id;
    struct mk_slab_t* p = c->buckets[id];
    
    if (p == s) {
        c->buckets[id] = s->next;
        mk_unmmap_l1((uint8_t *) s);

        if (SLAB_DEBUGGING) {
            print_str("[*] slab.c: reclaimed slab at vaddr ");
            print_qword((uint64_t) s);
        }

        return;
    }
    
    while (p != 0) {
        if (p->next != s) {
            p = p->next;
            continue;
        }

        p->next = s->next;
        mk_unmmap_l1((uint8_t *) s);
        
        if (SLAB_DEBUGGING) {
            print_str("[*] slab.c: reclaimed slab at vaddr ");
            print_qword((uint64_t) s);
        }

        return;
    }
}

void* mkmalloc (uint32_t s) {
    int id = get_bucket_id(s);
    void* p = 0;

    disable_interrupts();
    
    if (id >= NUM_BUCKETS) {
        print_error("[!] slab.c: mkmalloc size too large\n");
        enable_interrupts();

        return 0;
    }
    
    if (main_cache.buckets[id] == 0) 
        new_slab_push(&main_cache, id);
    
    p = alloc_from_slab(main_cache.buckets[id]);
    
    if (p == 0) {
        new_slab_push(&main_cache, id);
        p = alloc_from_slab(main_cache.buckets[id]);
    }
    
    enable_interrupts();

    return p;
}

void mkfree (void* p) {
    disable_interrupts();
    _mkfree (p);
    enable_interrupts();
}

void _mkfree (void* p) {
    struct mk_slab_t* header = (struct mk_slab_t *) ((uint64_t) p & ~0xfff);

    if (((uint64_t) p - ((uint64_t) header + sizeof(struct mk_slab_t)))
        % header->size != 0) {
        
        print_error("[!] slab.c: misaligned free\n");

        return;
    }
    
    *(void **) p = header->freelist_head;

    header->free_count++;
    header->freelist_head = p;
    
    if (SLAB_DEBUGGING) {
        print_str("[*] slab.c: freed slab obj, free count ");
        print_dword(header->free_count);
        print_char('\n');
    }
    
    if (header->free_count >= header->max) {
        reclaim_slab(&main_cache, header);
    }
}
