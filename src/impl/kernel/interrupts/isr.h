#pragma once

#include <stdint.h>
#include <stdbool.h>

void keyboard_int_handler(void);
void timer_int_handler(void);

char translate_scancode_set_1(uint8_t sc, bool shift);
