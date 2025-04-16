#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define UTILS_MAX(a, b) ((a) > (b) ? (a) : (b))

uint64_t hash(char *s);

#endif
