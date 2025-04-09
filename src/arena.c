#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <memory.h>

#include "arena.h"
#include "utils.h"

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

void arena_chunk_free(arena_chunk_t arena_chunk[static 1])
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

void* __arena_alloc(arena_t arena[static 1], ptrdiff_t size, ptrdiff_t align)
{     
    // ------------------
    // |  used  |  ...  |
    // ------------------
    // |        |         
    // *data    addr    
    // 
    // addr contiene l'indirizzo (data + used) 
    // convertito in un intero senza segno,
    // permettendo operazioni disponibili sugli
    // interi su quello che dovrebbe essere un indirizzo.
    // 
    // Nota: uintptr_t non e' portabile tra le architetture.
    uintptr_t addr = ((uintptr_t) arena->current->data) + arena->current->used;
    
    // l'indirizzo deve essere poi allineato (reso multiplo)
    // al numero di byte del tipo T a cui punta l'indirizzo,
    // che sarebbe _Alignof(T), questo viene fatto per motivi
    // di efficienza.
    uintptr_t aligned_addr = ARENA_ALIGN(addr, align);
    
    // (aligned_addr - addr) e' il padding, cioe'
    // la differenza tra l'indirizzo allineato e quello non.
    // La memoria che l'arena deve allocare e': padding + size. 
    ptrdiff_t needed = (aligned_addr - addr) + size;

    // Si sta richiedendo piu' memoria di quella disponibile dal chunk?
    if (arena->current->used + needed > arena->current->size) {
        
        // Si crea un altro chunk
        arena_chunk_t* chunk = arena_chunk_init(UTILS_MAX(arena->chunks->size * 2, size + align));
        if (!chunk) {
            return NULL;
        }
        
        // Si aggiunge alla lista concatenata
        chunk->next = arena->chunks;
        arena->chunks = chunk;
        arena->current = chunk;
        arena->total_size += chunk->size;
        arena->number_of_chunks++;

        // Si ricalcola l'indirizzo d'allineamento
        addr = (uintptr_t) chunk->data;
        aligned_addr = ARENA_ALIGN(addr, align);
        needed = (aligned_addr - addr) + size;

    }

    // Si restituisce il puntatore 
    // all'indirizzo allineato.
    void* ptr = (void*) aligned_addr;
    arena->current->used += needed;
    return ptr;    
}

void arena_free(arena_t arena[static 1])
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