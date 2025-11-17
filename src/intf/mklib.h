#pragma once

#include <stdint.h>

void memset(void* p, char c, uint32_t n);
int strcmp(char* s1, char* s2);
uint32_t strlen(char* s);
int strchr(char* s, char c);