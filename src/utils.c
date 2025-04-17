#include "utils.h"

uint64_t hash(char const*const s)
{
    uint64_t h = 0x100;
	const uint64_t prime = 1111111111111111111u;
    for (const uint8_t* p = (const uint8_t*) s; *p; ++p) {
        h ^= *p;
        h *= prime;
    }
    return h;
}
