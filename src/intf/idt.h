#pragma once

#include <stdint.h>
#include <stdbool.h>

#define GDT_OFFSET_KERNEL_CODE 0x8
#define IDT_MAX_DESCRIPTORS 0x100

// ==================================

void mk_idt_init(void);

static bool vectors[IDT_MAX_DESCRIPTORS];

extern void* mk_isr_stub_table[];
extern void* mk_asm_keyboard_int;
extern void* mk_asm_timer_int;

// ==================================

typedef struct {
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t	    ist;          // The IST (interrupt stack table) in the TSS that the CPU will load into RSP; set to zero for now
	uint8_t     attributes;   // lower nibble = gate type (0xE specifies interrupt gate) next bit is 0, next 2 bits are priv levels and last bit is present bit (set to 1 to enable)
	uint16_t    isr_mid;      // The higher 16 bits of the lower 32 bits of the ISR's address
	uint32_t    isr_high;     // The higher 32 bits of the ISR's address
	uint32_t    reserved;     // Set to zero
} __attribute__((packed)) idt_entry_t;

// this is for the IDT register
typedef struct {
	uint16_t	limit;
	uint64_t	base;
} __attribute__((packed)) idtr_t;

