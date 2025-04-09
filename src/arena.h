#ifndef __ARENA_H__
#define __ARENA_H__

#include <stddef.h>
#include <stdint.h>

/*

  -----------------------------------------------------------------------------------
  | ----------------    -------------------           ----------------------------- |
  | | chunk (64kb) | -> | chunk (>=128kb) | -> ... -> | chunk (>=(prevChunk * 2)) | |
  | ----------------    -------------------           ----------------------------- |
  -----------------------------------------------------------------------------------
  |
  Arena

*/

#define ARENA_FAILED_ME  (1 << 7)
#define ARENA_CHUNK_SIZE (64 * 1024)

#define arena_alloc(a, t, n) (__arena_alloc(a, sizeof(t) * n, _Alignof(t)))

typedef struct arena_chunk arena_chunk_t;
typedef struct arena arena_t;

struct arena_chunk {       
    void*          data;
    ptrdiff_t      size;
    ptrdiff_t      used;
    arena_chunk_t* next;
};

struct arena {
    // Puntatore al chunk corrente.
    arena_chunk_t* current;
    
    // Puntatore alla testa della lista dei chunk. 
    arena_chunk_t* chunks;

    // Dimensione totale utilizzata dall'arena.
    ptrdiff_t      total_size;

    // Numero di chunk.
    ptrdiff_t      number_of_chunks;
    
    // Flags errori dell'arena.
    // ex: 
    // - flags |= ERR_FLAGS;
    // - if (flags & ERR_FLAGS) { ... }
    uint8_t        flags;
};

arena_chunk_t* arena_chunk_init(ptrdiff_t size);
void           arena_chunk_free(arena_chunk_t* arena_chunk);
arena_t        arena_init();
void*          __arena_alloc(arena_t* arena, ptrdiff_t size, ptrdiff_t align);
void           arena_free(arena_t* arena);

#endif // ARENA_H