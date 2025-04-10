#include <stdio.h>
#include "arena.h"
#include "tokenizer.h"

int main()
{
	arena_t arena = arena_init();
	
	token_t* tks = get_tokens(
		"In Borges's enigmatic short story 'The Library of Babel', the labyrinth—filled with state-of-the-art, mind-bending corridors, twist-laden passages, and self-referential paradoxes—doesn't merely confound its characters; instead, it challenges readers (e.g., those who believe reality is linear) by suggesting that life's intricate, ever-evolving narrative is as unpredictable as a well-crafted, meta-fictional riddle.", 
		&arena
	);

	for (token_t* node = tks; node != NULL; node = node->next) 
		printf("%s ", node->buff);
	putchar('\n');

	arena_free(&arena);
	return 0;
}
