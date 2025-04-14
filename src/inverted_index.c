#include <string.h>
#include "inverted_index.h"
#include "arena.h"
#include <assert.h>

static inline uint64_t hash(char *s)
{
    uint64_t h = 0x100;
    for (size_t i = 0; i < strlen(s); ++i) {
        h ^= s[i];
        h *= 1111111111111111111u;
    }
    return h;
}

posting_t** inv_idx_upsert(inverted_index_t **index, char term[MAX_TERM_LEN], arena_t *arena)
{
    assert(term[strlen(term)] == 0);

    for (uint64_t h = hash(term); *index; h <<= 2) {
        if (strncmp(term, (*index)->term, MAX_TERM_LEN)) {
            return &(*index)->postings;
        }
        index = &(*index)->child[h >> 62];
    }

    *index = arena_alloc(arena, inverted_index_t, 1);
    strcpy((*index)->term, term);

    assert((*index)->term[strlen((*index)->term)] == 0);

    return &(*index)->postings;
}

void posting_insert(posting_t** postings, posting_t* posting)
{
    posting->next = *postings;
    *postings = posting;
}

void inv_idx_ps_ins(inverted_index_t** idx, char term[MAX_TERM_LEN], posting_t* posting, arena_t* arena)
{
    posting_insert(inv_idx_upsert(idx, term, arena), posting);
}