// Di seguito l'implementazione di un tokenizatore di base
// per la lingua inglese.
// 
// Prendiamo il seguente paragrafo per illustrare il funzionamento:
// 
// "In Borges's enigmatic short story 'The Library of Babel', 
// the labyrinth—filled with state-of-the-art, mind-bending corridors, 
// twist-laden passages, and self-referential paradoxes—doesn't merely confound its characters;
// instead, it challenges readers (e.g., those who believe reality is linear) 
// by suggesting that life's intricate, ever-evolving narrative is as unpredictable 
// as a well-crafted, meta-fictional riddle."
// 
// 1) Conversione in minuscolo
//  
// "in borges's enigmatic short story 'the library of babel', 
// the labyrinth—filled with state-of-the-art, mind-bending corridors, 
// twist-laden passages, and self-referential paradoxes—doesn't merely confound its characters;
// instead, it challenges readers (e.g., those who believe reality is linear) 
// by suggesting that life's intricate, ever-evolving narrative is as unpredictable 
// as a well-crafted, meta-fictional riddle."
//
// 2) Si rimuove poi, la maggior parte della punteggiatura, 
// lasciando solo gli elementi che aggiungono importanza semantica,
// come per esempio "don't", "won't" o "can't". Il genitivo sassone 
// e' un caso particolare, il "'s" verra' declinato, quindi "borges's"
// diventera' "borges". 
// 
// "in borges enigmatic short story the library of babel
// the labyrinth filled with state of the art mind bending corridors 
// twist laden passages and self referential paradoxes doesn't merely confound its characters
// instead it challenges readers eg those who believe reality is linear 
// by suggesting that life intricate ever evolving narrative is as unpredictable 
// as a well crafted meta fictional riddle"
// 
// 3) Divisione in token
//
// "[in] [borges] [enigmatic] [short] [story] [the] [library] [of] [babel]
// [the] [labyrinth] [filled] [with] [state] [of] [the] [art] [mind] [bending] [corridors]
// [twist] [laden] [passages] [and] [self] [referential] [paradoxes] [doesn't] [merely] [confound] 
// [its] [characters] [instead] [it] [challenges] [readers] [eg] [those] [who] [believe] [reality]
// [is] [linear] [by] [suggesting] [that] [life] [intricate] [ever] [evolving] [narrative] [is] 
// [as] [unpredictable] [as] [a] [well] [crafted] [meta] [fictional] [riddle]"
//
// Questa e' naturalmente un'implementazione basilare, tuttavia considero il processo di tokenizzazione
// la parte meno interessante di un motore di ricerca.

#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "arena.h"

#define MAX_TOKEN_SIZE 256

typedef struct token token_t;
typedef struct token_pool token_pool_t;
typedef struct token_position token_position_t;

struct token_position {
	token_position_t* next;
	uint32_t pos;
};	

struct token {
    token_t* next;
    token_position_t* positions;
    uint32_t frequency;
    char buff[MAX_TOKEN_SIZE];    
};

struct token_pool {	
	token_t* token;		
	char term[MAX_TOKEN_SIZE];
	token_pool_t* child[4];
};

token_t* get_tokens(FILE* fp, token_pool_t** pool, arena_t arena[static 1]);

#endif // __TOKENIZER__
