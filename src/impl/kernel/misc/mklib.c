#include "mklib.h"
#include <stdint.h>

void _memset(void *p, char c, uint32_t n) {
    for (uint32_t i = 0; i < n; i++)
        *(char *) p++ = c;
}

int _strcmp(char *s1, char *s2) {
    int off = 0;
    
    while (*(s1 + off) != 0) {
        int n = *(s1 + off) - *(s2 + off);
        off++;

        if (n != 0)
            return n;
    }
    
    return 0;
}

uint32_t _strlen(char* s) {
    int off = 0;

    while (*(s + off) != 0) {
        off++;
    }
    
    return off;
}

int _strchr(char *s, char c) {
    int off = 0;

    while (*(s + off) != 0) {
        if (*(s + off) == c)
            return off;

        off++;
    }
    
    return -1;
}
