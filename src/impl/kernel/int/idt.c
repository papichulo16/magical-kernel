#include "idt.h"
#include "pic.h"

__attribute__((aligned(0x10))) 
static idt_entry_t idt[256]; // Create an array of IDT entries; aligned for performance

static idtr_t idtr;

// this is to set up an idt entry
void mk_idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low        = (uint64_t)isr & 0xFFFF;
    descriptor->kernel_cs      = GDT_OFFSET_KERNEL_CODE;
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->isr_mid        = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high       = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved       = 0;
}

void mk_idt_init() {
    // set up the interrupt descriptor table register info
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    // fill the IDT for the exception handlers
    for (uint8_t vector = 0; vector < 32; vector++) {
        mk_idt_set_descriptor(vector, mk_isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }
    
    mk_idt_set_descriptor(0x20, &mk_asm_timer_int, 0x8E);
    mk_idt_set_descriptor(0x21, &mk_asm_keyboard_int, 0x8E);
    
    // set the IDTR
    __asm__ volatile ("lidt %0" : : "m"(idtr)); // load the new IDT

    // enable interrupts
    __asm__ volatile ("sti"); // set the interrupt flag
    
    mk_pit_init(100); // init timer
}

