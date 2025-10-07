#pragma once

#include <stdint.h>
#include <stdbool.h>

__attribute__((noreturn)) void mk_exception_handler(void);

void mk_keyboard_int_handler(void);
void mk_timer_int_handler(void);

char translate_scancode_set_1(uint8_t sc, bool shift);
