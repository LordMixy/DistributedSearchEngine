// Idee teoriche: https://nlp.stanford.edu/IR-book/pdf/irbookonlinereading.pdf
//
// Il Reverse Index e' una struttura dati composta
// da un dizionario che contiene delle parole dette "term".
// Per ogni parola, si ha una lista di documenti
// dove la parola appare.
// Il documento e' identificato da un id, detto DocID,
// una tupla (term, DocID) viene chimata posting.
//
// Hash-Trie: https://nullprogram.com/blog/2023/09/30/
// Test Push

#ifndef __INVERTED_INDEX_H__
#define __INVERTED_INDEX_H__

#include <stdint.h>
#include "arena.h"

#define MAX_TERM_LEN 256

typedef struct posting posting_t;
typedef struct inverted_index inverted_index_t;

struct posting {
    uint64_t   doc_id;
    uint64_t   position;    
    uint64_t   frequency;
    posting_t* next;
};

struct inverted_index {
    char              term[MAX_TERM_LEN];
    posting_t*        postings;
    inverted_index_t* child[4];
};

posting_t** inv_idx_upsert(inverted_index_t**, char[MAX_TERM_LEN], arena_t*);
void        inv_idx_ps_ins(inverted_index_t **, char[MAX_TERM_LEN], posting_t*, arena_t*);
void        posting_insert(posting_t **, posting_t *);

#endif // __INVERTED_INDEX_H__
