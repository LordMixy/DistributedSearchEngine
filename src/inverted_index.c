#include <stdio.h>
#include <assert.h>

#include "utils.h"
#include "inverted_index.h"
#include "arena.h"

posting_t** inv_idx_upsert(inverted_index_t** index, char term[MAX_TERM_LEN], arena_t* arena)
{
	// h = (h * 4) ^ (h >> 60)
    for (uint64_t h = hash(term); *index; h <<= 2) {
    	if (!strncmp(term, (*index)->term, MAX_TERM_LEN)) {
        	return &(*index)->postings;
        }
        index = &(*index)->child[(h >> 62)];
    }

    *index = arena_alloc(arena, inverted_index_t, 1);
    memset(*index, 0, sizeof(inverted_index_t));
    strncpy((*index)->term, term, MAX_TERM_LEN);
    (*index)->term[strlen(term)] = 0;

    return &(*index)->postings;
}

posting_t* get_postings(inverted_index_t* index, char term[MAX_TERM_LEN])
{
    for (uint64_t h = hash(term); index; h <<= 2) {
    	if (!strncmp(term, (index)->term, MAX_TERM_LEN)) {
        	return (index)->postings;
        }
        index = (index)->child[(h >> 62)];
    }
    return NULL;
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

posting_t* posting_init(uint64_t doc_id, uint64_t frequency, token_position_t* positions, arena_t* arena)
{
    posting_t* p = arena_alloc(arena, posting_t, 1);
    *p = (posting_t) {
        .doc_id = doc_id,
        .frequency = frequency,
        .positions = positions, 
    };
    return p;
}
