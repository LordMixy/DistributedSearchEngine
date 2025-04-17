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
#include "utils.h"

typedef struct token token_t;
typedef struct token_pool token_pool_t;
typedef struct token_position token_position_t;

struct token_position {
	token_position_t* next;
	uint32_t pos;
};	

// Nota: il campo next e' il primo
// per motivi di efficienza:
// quando la funzione token_insert 
// viene chiamata, per caricare il campo next 
// in memoria viene eseguita l'istruzione
// MOV QWORD PTR [rax + 0x100], rdx,
// per leggere rax + 0x100 si saltano 4 cache line.
// Secondo perf questo vuol dire che che la fuzione prendeva
// il 35% del tempo del programma per via dei cache misses.
// Spostando il campo next in cima, l'istruzione diventa:
// MOV QWORD PTR [rax], rdx, 
// facendo scendere la percetuale a 3%.
struct token {
    token_t* next;
    token_position_t* positions;
	token_pool_t* owner;
};

// Per evitare di creare un token per le parole 
// gia' lette, un pool e' il modo migliore di far
// diminuire il numero di allocazioni e di memoria 
// utilizzata dal programma.
struct token_pool {	
	token_t* token;		
	uint32_t frequency;
	token_pool_t* child[4];
	char term[MAX_WORD_LEN];
};

token_t* get_tokens(FILE* fp, token_pool_t** pool, arena_t arena[static 1]);

#endif // __TOKENIZER__
