#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#include "tokenizer.h"
#include "utils.h"

static inline void token_insert(token_t** list, token_t* node)  
{ 
    node->next = *list; 
    *list = node; 
} 

static inline void token_position_insert(token_position_t** list, token_position_t* node)  
{ 
    node->next = *list; 
    *list = node; 
} 

// TODO una tabella hash e' notevolmente
// una decisione migliore
struct {
    const char* string;
    ptrdiff_t length;
} contractions[] = {
    {"is", 2},            // da "isn't"
    {"are", 3},           // da "aren't"
    {"was", 3},           // da "wasn't"
    {"were", 4},          // da "weren't"
    {"do", 2},            // da "don't"
    {"does", 4},          // da "doesn't"
    {"did", 3},           // da "didn't"
    {"ca", 2},            // da "can't"
    {"could", 5},         // da "couldn't"
    {"wo", 2},            // da "won't" (irregolare per "will not")
    {"would", 5},         // da "wouldn't"
    {"should", 6},        // da "shouldn't"
    {"mus", 3},           // da "mustn't"
    {"migh", 4},          // da "mightn't"
    {"need", 4},          // da "needn't"
    {"ough", 4},          // da "oughtn't"
    {"had", 3},           // da "hadn't"
    {"have", 4},          // da "haven't"
    {"has", 3},           // da "hasn't"
    {"dare", 4},          // da "daren't"
    {"ai", 2},            // da "ain't"
    {"may", 3},           // da "mayn't"
    {"sha", 3},           // da "shan't"
    {"end", -1}
};

static token_t* token_init(token_pool_t* owner, arena_t* arena)
{
    token_t* tk = arena_alloc(arena, token_t, 1);

	tk->owner = owner;
    tk->positions = NULL;
    tk->next = NULL;

    return tk;
}

static token_position_t* token_position_init(uint32_t pos, arena_t* arena)
{
    token_position_t* tk = arena_alloc(arena, token_position_t, 1);
	tk->pos = pos;
	tk->next = NULL;
    return tk;
}

token_position_t** token_pool_upsert(token_pool_t** pool, token_t** tokens, const char term[MAX_WORD_LEN], arena_t* arena)
{
    for (uint64_t h = hash(term); *pool; h <<= 2) {
    	if (!strncmp(term, (*pool)->term, MAX_WORD_LEN)) {
			(*pool)->frequency++;
        	return &(*pool)->token->positions;
        }
        pool = &(*pool)->child[h >> 62];
    }

    *pool = arena_alloc(arena, token_pool_t, 1);
    memset(*pool, 0, sizeof(token_pool_t));

    strncpy((*pool)->term, term, MAX_WORD_LEN);
    (*pool)->term[strlen(term)] = 0;

	(*pool)->frequency = 1;
	
	(*pool)->token = token_init((*pool), arena);
	token_insert(tokens, (*pool)->token);
	
    return &(*pool)->token->positions;
}

token_t* get_tokens(FILE* fp, token_pool_t** pool, arena_t arena[static 1])
{
    token_t* tokens = NULL;
    size_t currentLen = 0;
    size_t pos = 0;
    char token[MAX_WORD_LEN];
    for (int32_t c; (c = fgetc(fp)) != EOF; ++pos) {
        // Se il carattere e' alfanumerico, si aggiunge al token 
        if (isalnum(c)) token[currentLen++] = tolower(c);
        // Se il carattere e' un apostrofo, si controlla
        // se fa parte di una forma contratta, come "don't"
        else if (c == '\'') {
            size_t j;
            // Si cerca la forma contratta
            for (j = 0; contractions[j].length != -1 && strncmp(token, contractions[j].string, contractions[j].length); ++j);
            // Se esiste, la si aggiunge
            if (contractions[j].length > 0) 
                token[currentLen++] = tolower(c);
        } else if (currentLen > 0 && token[currentLen - 1]) {
            token[currentLen] = '\0';            
            if (*token) {
            	token_position_t** token_ins = token_pool_upsert(pool, &tokens, token, arena);
				token_position_insert(token_ins, token_position_init(pos, arena));
            }
            currentLen = 0;
        }
    }
    return tokens;
}
