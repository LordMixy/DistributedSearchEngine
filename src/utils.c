#include "utils.h"

uint64_t hash(char *s)
{
    uint64_t h = 0x100;
    for (size_t i = 0; i < strlen(s); ++i) {
        h ^= s[i];
        h *= 1111111111111111111u;
    }
    return h;
}
