#include "../includes/print.h"
#include "../includes/inlines.c"

#include "pic.h"
#include "isr.h"

bool shift = false;

void timer_int_handler() {
    print_str("AHHHH");
    
    PIC_sendEOI(0);
}

void keyboard_int_handler() {
    // status register bit 1 = output  buffer status
    if ((inb(0x64) & 1) == 0)
        return;
    
    uint8_t scancode = inb(0x60); // read the data port
    
    // ignore key releases
    if (scancode & 0x80) {
        scancode &= 0x7F;
        if (scancode == 0x2A || scancode == 0x36)
            shift = false;
    }
    else {
        if (scancode == 0x2A || scancode == 0x36) {
            shift = true;
            PIC_sendEOI(1);

            return;
        }

        print_char(translate_scancode_set_1(scancode, shift));
    }

    PIC_sendEOI(1);
}
