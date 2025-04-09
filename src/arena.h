#ifndef __ARENA_H__
#define __ARENA_H__

#include <stddef.h>

typedef struct arena_region arena_region;
typedef struct arena arena;

struct arena_region {
	void* data;
	ptrdiff_t used;
	ptrdiff_t capacity;
	arena_region* next;
};

struct arena {
	arena_region* beg;
	arena_region* end;
};


#endif // ARENA_H