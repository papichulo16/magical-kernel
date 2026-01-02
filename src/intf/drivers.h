#pragma once

#define INTEL_VEND_ID 0x8086

#define ICH9_AHCI_DEV 0x2922

void mk_ahci_init();

void mk_ps2_keyboard_driver();
char translate_scancode_set_1(uint8_t sc, bool shift);
