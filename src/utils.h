#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define UTILS_MAX(a, b) ((a) > (b) ? (a) : (b))

#define MAX_WORD_LEN 256

uint64_t hash(char const*const s);

#endif
