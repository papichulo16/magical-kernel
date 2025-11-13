#include "virt.h"

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

uint64_t get_cr3() {
    uint64_t cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    
    return cr3;
}

void* phys_to_kern_virt(uint8_t* paddr) {
    return (uint64_t) KERNEL_VMA + paddr;
}

void mk_virt_init() {
    uint64_t* cr3 = phys_to_kern_virt((uint8_t*) get_cr3());

    // clear the first entries from L4 and L3, they are doubled up
    uint64_t* l3_table = (uint64_t *) ((uint64_t) phys_to_kern_virt((uint8_t *) cr3[0]) & ~0xfff);
    
    l3_table[0] = 0;
    cr3[0] = 0;
}

