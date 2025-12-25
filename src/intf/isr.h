#pragma once

#include <stdint.h>
#include <stdbool.h>

extern void* keyboard_sema;

__attribute__((noreturn)) void mk_exception_handler(uint64_t stack, uint64_t vector);
void halt();

void mk_timer_int_handler(uint64_t* stack);
void mk_keyboard_int_handler(void);

