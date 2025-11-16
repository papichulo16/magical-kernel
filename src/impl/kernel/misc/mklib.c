#include "mklib.h"

void memset(void *p, char c, uint32_t n) {
    for (uint32_t i = 0; i < n; i++)
        *(char *) p++ = c;
}
