#include "virt.h"
#include "page.h"
#include "mklib.h"

#include <stdint.h>

/*

Bit 0:  Present (P)        - Must be 1 for valid entry
Bit 1:  Read/Write (R/W)   - 0=read-only, 1=read-write  
Bit 2:  User/Supervisor    - 0=kernel only, 1=user accessible
Bit 3:  Write-Through (PWT)
Bit 4:  Cache Disable (PCD)
Bit 5:  Accessed (A)       - Set by CPU when accessed
Bit 6:  Dirty (D)          - Set by CPU when written (only in final level)
Bit 7:  Page Size (PS)     - 1=huge page (stop here), 0=continue to next level
Bit 8:  Global (G)         - Don't flush from TLB on CR3 reload
Bits 9-11: Available       - OS can use for whatever

*/

#define PRESENT 1
#define WRITE 1 << 1
#define USER 1 << 2
#define HUGE 1 << 7

#define L4_INDEX(va)   (((va) >> 39) & 0x1ff)
#define L3_INDEX(va)   (((va) >> 30) & 0x1ff)
#define L2_INDEX(va)   (((va) >> 21) & 0x1ff)
#define L1_INDEX(va)   (((va) >> 12) & 0x1ff)

uint64_t KERNEL_SIZE;
uint64_t KERNEL_DATA_VMA;

uint64_t* get_cr3() {
    uint64_t* cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    
    return cr3;
}

void* phys_to_kern_virt(uint8_t* paddr) {
    return (uint64_t) KERNEL_VMA + paddr;
}

uint64_t* get_l3_table(uint64_t p) {
    uint64_t* cr3 = get_cr3();
    uint64_t idx = L4_INDEX(p);
    
    return (uint64_t *) (cr3[idx] & ~0xfff);
}

void alloc_l3_table(uint64_t p) {
    uint64_t* cr3 = get_cr3();
    uint64_t idx = L4_INDEX(p);
    
    if (cr3[idx] != 0) 
        return;

    cr3[idx] = (uint64_t) mk_phys_page_alloc() | PRESENT | WRITE;
    memset((void *) (cr3[idx] & ~0xfff), 0, PAGE_SIZE);
}

uint64_t* get_l2_table(uint64_t p) {
    uint64_t* l3 = get_l3_table(p);
    uint64_t idx = L3_INDEX(p);
    
    if (!l3)
        return 0;

    return (uint64_t *) (l3[idx] & ~0xfff);
}

void alloc_l2_table(uint64_t p) {
    uint64_t* l3 = get_l3_table(p);
    uint64_t idx = L3_INDEX(p);
    
    if (l3[idx] != 0) 
        return;

    l3[idx] = (uint64_t) mk_phys_page_alloc() | PRESENT | WRITE;
    memset((void *) (l3[idx] & ~0xfff), 0, PAGE_SIZE);
}

uint64_t* get_l1_table(uint64_t p) {
    uint64_t* l2 = get_l2_table(p);
    uint64_t idx = L2_INDEX(p);

    if (!l2)
        return 0;

    return (uint64_t *) (l2[idx] & ~0xfff);
}

void alloc_l1_table(uint64_t p) {
    uint64_t* l2 = get_l2_table(p);
    uint64_t idx = L2_INDEX(p);
    
    if (l2[idx] != 0) 
        return;

    l2[idx] = (uint64_t) mk_phys_page_alloc() | PRESENT | WRITE;
    memset((void *) (l2[idx] & ~0xfff), 0, PAGE_SIZE);
}


uint64_t* get_l1_idx(uint64_t p) {
    uint64_t* l1 = get_l1_table(p);
    uint64_t idx = L1_INDEX(p);

    if (!l1)
        return (uint64_t *) 1;

    return (uint64_t *) (l1[idx] & ~0xfff);
}

uint8_t* kern_get_next_free_l1_addr(uint8_t* p){
    
    // they wont allocate if the index already exists
    alloc_l3_table((uint64_t) p);
    alloc_l2_table((uint64_t) p);
    alloc_l1_table((uint64_t) p);
    
    uint64_t* l1 = get_l1_table((uint64_t) p);
    
    for (; L1_INDEX((uint64_t) p) < 512; p += (1 << 12)) {
        if (get_l1_idx((uint64_t) p) == 0)
            return p;
    }

    // try again with a new l1 table index
    return kern_get_next_free_l1_addr(p + (1 << 12));
}

int map_l1(uint64_t p) {
    if (get_l1_idx(p))
        return -1;
    
    uint64_t* l1 = get_l1_table(p);
    l1[L1_INDEX(p)] = (uint64_t) mk_phys_page_alloc() | PRESENT | WRITE;

    return 0;
}

uint8_t* mk_vmmap_l1(uint8_t flags) {
    uint8_t* p = kern_get_next_free_l1_addr((uint8_t *) KERNEL_DATA_VMA);
    
    if (!map_l1((uint64_t) p))
        return p;
    
    return 0;
}

void mk_unmmap_l1(uint8_t *vaddr) {
    uint8_t** l1 = (uint8_t **) get_l1_table((uint64_t) vaddr);
    
    if (l1 == 0)
        return;
    
    int idx = L1_INDEX((uint64_t) vaddr);
    uint8_t* paddr = (uint8_t *) ((uint64_t) l1[idx] & ~0xfff); 

    mk_phys_page_free(paddr);
    l1[idx] = 0;
}

void mk_virt_init() {
    uint64_t* cr3 = phys_to_kern_virt((uint8_t*) get_cr3());

    /* 
     
    TOO MUCH PAIN IN THE ASS DEALING WITH ALL VIRTUAL ADDRESSES
    CALL ME A BITCH, BUT THIS IS GONNA MAKE MY LIFE A LOT EASIER

    // clear the first entries from L4 and L3, they are doubled up
    uint64_t* l3_table = (uint64_t *) ((uint64_t) phys_to_kern_virt((uint8_t *) cr3[0]) & ~0xfff);
    
    l3_table[0] = 0;
    cr3[0] = 0;

    */
    
    KERNEL_SIZE = (((uint64_t) KERNEL_VIRT_END - (uint64_t) KERNEL_VIRT_START) & ~0xfff) + 0x1000;
    KERNEL_DATA_VMA = (uint64_t) KERNEL_VMA + (uint64_t) KERNEL_SIZE;
}
