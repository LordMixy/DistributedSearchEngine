#include <stdio.h>
#include <assert.h>
#include "arena.h"
#include "tokenizer.h"
#include "inverted_index.h"

int main()
{
	arena_t arena = arena_init();
	
	token_t* tks = get_tokens(
		"In Borges's enigmatic short story 'The Library of Babel', the labyrinth—filled with state-of-the-art, mind-bending corridors, twist-laden passages, and self-referential paradoxes—doesn't merely confound its characters; instead, it challenges readers (e.g., those who believe reality is linear) by suggesting that life's intricate, ever-evolving narrative is as unpredictable as a well-crafted, meta-fictional riddle.", 
		&arena
	);

	inverted_index_t* idx = NULL;
	for (token_t* node = tks; node != NULL; node = node->next) {		
        posting_t* p = arena_alloc(&arena, posting_t, 1);
        *p = (posting_t) {
            .doc_id = 42,
            .frequency = 42,
            .position = 42
        };
        inv_idx_ps_ins(&idx, node->buff, p, &arena);
	}

    for (token_t* node = tks; node != NULL; node = node->next) {
        posting_t** ps = inv_idx_upsert(&idx, node->buff, &arena);
        if (*ps) printf("%lu\n", (*ps)->doc_id);
        else puts(node->buff);
    }

	arena_free(&arena);
	return 0;
}
