#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <memory.h>

#include "arena.h"
#include "utils.h" // DSE_MAX

#define ARENA_ALIGN(addr, align) (((addr) + (align) - 1) & ~((align) - 1))

arena_chunk_t* arena_chunk_init(ptrdiff_t size)
{
    arena_chunk_t* arena_chunk = malloc(sizeof *arena_chunk);
    if (arena_chunk) {
        arena_chunk->data = malloc(size);
        if (!arena_chunk->data) {
            free(arena_chunk);
            return NULL;
        }
        arena_chunk->size = size;
        arena_chunk->used = 0;
        arena_chunk->next = NULL;
    }
    return arena_chunk;
}

void arena_chunk_free(arena_chunk_t* arena_chunk)
{
    free(arena_chunk->data);
    free(arena_chunk);
}

arena_t arena_init()
{
    arena_t arena = {0};
    arena_chunk_t* arena_chunk = arena_chunk_init(ARENA_CHUNK_SIZE);

    if (arena_chunk) {
        arena.chunks = arena_chunk;
        arena.current = arena_chunk;
        arena.number_of_chunks = 1;
        arena.total_size = ARENA_CHUNK_SIZE;
    } else {
        arena.flags |= ARENA_FAILED_ME;
    }

    return arena;
}

void* __arena_alloc(arena_t* arena, ptrdiff_t size, ptrdiff_t align)
{
    uintptr_t addr = ((uintptr_t) arena->current->data) + arena->current->used;
    uintptr_t aligned_addr = ARENA_ALIGN(addr, align);

    ptrdiff_t padding = aligned_addr - addr;
    ptrdiff_t needed = padding + size;

    if (arena->current->used + needed > arena->current->size) {
        arena_chunk_t* chunk = arena_chunk_init(UTILS_MAX(arena->chunks->size * 2, size + align));

        // TODO il chunk e' stato inizilizzato correttamente?
    	  
        chunk->next = arena->chunks;
        arena->chunks = chunk;
        arena->current = chunk;
        arena->total_size += chunk->size;
        arena->number_of_chunks++;

        addr = (uintptr_t) chunk->data;
        aligned_addr = ARENA_ALIGN(addr, align);

        padding = aligned_addr - addr;
        needed = padding + size;
    }

    void* ptr = (void*) aligned_addr;
    arena->current->used += needed;
    return ptr;    
}

void arena_free(arena_t* arena)
{
    arena_chunk_t* chunk = arena->chunks;
    while (chunk) {
        arena_chunk_t* next = chunk->next;
        arena_chunk_free(chunk);
        chunk = next;
    }

    arena->chunks = NULL;
    arena->current = NULL;
    arena->total_size = 0;
    arena->number_of_chunks = 0;
}