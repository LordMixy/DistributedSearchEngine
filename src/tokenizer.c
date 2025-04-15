#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "tokenizer.h"

#define MAX_TOKEN_SIZE 256

struct {
    char*     string;
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

static token_t* token_init(char* str, arena_t* arena)
{
    token_t* tk = arena_alloc(arena, token_t, 1);
    
    size_t len = strlen(str);
    memcpy(tk->buff, str, len);
    tk->buff[len] = 0;

    return tk;
}

static void token_insert(token_t** list, token_t* node)
{
    node->next = *list;
    *list = node;
}

token_t* get_tokens(FILE* fp, arena_t arena[static 1])
{
    token_t* tokens = NULL;
    size_t currentLen = 0;
    
    char token[MAX_TOKEN_SIZE];
    int32_t c;

    while ((c = fgetc(fp)) != EOF)
    {
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
        } 
        
        // ...
        else if (token[currentLen - 1]) {
            token[currentLen] = '\0';            
            if (*token) 
                token_insert(&tokens, token_init(token, arena));
            currentLen = 0;
        }
    }

    return tokens;
}
