#pragma once

#include <stdint.h>

void _memset(void* p, char c, uint32_t n);
int _strcmp(char* s1, char* s2);
uint32_t _strlen(char* s);
int _strchr(char* s, char c);
