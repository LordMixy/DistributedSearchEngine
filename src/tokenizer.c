#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
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

__attribute__((always_inline)) // TODO da rimuovere chiaramente
static inline void token_insert(token_t** list, token_t* node)
{
    node->next = *list;
    *list = node;
}

// token_t* get_tokens(FILE* fp, arena_t arena[static 1])
// {
//     token_t* tokens = NULL;
//     size_t currentLen = 0;
    
//     char token[MAX_TOKEN_SIZE];
//     int32_t c;

//     while ((c = fgetc(fp)) != EOF)
//     {
//         // Se il carattere e' alfanumerico, si aggiunge al token 
//         if (isalnum(c)) token[currentLen++] = tolower(c);

//         // Se il carattere e' un apostrofo, si controlla
//         // se fa parte di una forma contratta, come "don't"
//         else if (c == '\'') {
//             size_t j;
            
//             // Si cerca la forma contratta
//             for (j = 0; contractions[j].length != -1 && strncmp(token, contractions[j].string, contractions[j].length); ++j);
            
//             // Se esiste, la si aggiunge
//             if (contractions[j].length > 0) 
//                 token[currentLen++] = tolower(c);
//         } 
        
//         // ...
//         else if (currentLen > 0 && token[currentLen - 1]) {
//             token[currentLen] = '\0';            
//             if (*token) 
//                 token_insert(&tokens, token_init(token, arena));
//             currentLen = 0;
//         }
//     }

//     return tokens;
// }

#define BUFFER_SIZE 8192

static const unsigned char tolower_table[256] = 
{
    // 0x00 - 0x40 (caratteri non alfabetici)
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40,

    // 0x41-0x5A (A-Z) → 0x61-0x7A (a-z)
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x79, 0x7A,

    // 0x5B-0x60 (caratteri non alfabetici)
    0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60,

    // 0x61-0x7A (a-z, già minuscoli)
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x79, 0x7A,

    // 0x7B-0xFF (caratteri non alfabetici, inclusi i caratteri estesi)
    0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81, 0x82,
    0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A,
    0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92,
    0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A,
    0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2,
    0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA,
    0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2,
    0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA,
    0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2,
    0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA,
    0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2,
    0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
    0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1, 0xE2,
    0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA,
    0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2,
    0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA,
    0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

static const bool is_alphanumeric[256] = {
    /* 0x00 - 0x0F */
    false, false, false, false, false, false, false, false, 
    false, false, false, false, false, false, false, false,
    /* 0x10 - 0x1F */
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /* 0x20 - 0x2F (space through /) */
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /* 0x30 - 0x39 (digits 0-9) */
    true,  true,  true,  true,  true,  true,  true,  true,
    true,  true,  false, false, false, false, false, false,
    /* 0x40 - 0x5A (@ and uppercase A-Z) */
    false, true,  true,  true,  true,  true,  true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,
    /* 0x50 - 0x5F (uppercase P-Z and some symbols) */
    true,  true,  true,  true,  true,  true,  true,  true,
    true,  true,  true,  false, false, false, false, false,
    /* 0x60 - 0x6F (` and lowercase a-o) */
    false, true,  true,  true,  true,  true,  true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,
    /* 0x70 - 0x7F (lowercase p-z and some symbols) */
    true,  true,  true,  true,  true,  true,  true,  true,
    true,  true,  true,  false, false, false, false, false,
    /* 0x80 - 0x8F (extended ASCII) */
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /* 0x90 - 0x9F (extended ASCII) */
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /* 0xA0 - 0xAF (extended ASCII) */
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /* 0xB0 - 0xBF (extended ASCII) */
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /* 0xC0 - 0xCF (extended ASCII) */
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /* 0xD0 - 0xDF (extended ASCII) */
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /* 0xE0 - 0xEF (extended ASCII) */
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /* 0xF0 - 0xFF (extended ASCII) */
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false
};

token_t *get_tokens(FILE *fp, arena_t arena[static 1]) 
{
    token_t *tokens = NULL;
    size_t currentLen = 0;

    char token[MAX_TOKEN_SIZE];
    int32_t c;

	char buffer[BUFFER_SIZE];
	size_t bytes_read;

	while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)))
	{
	    for (size_t buffer_pos = 0; buffer_pos < bytes_read; ++buffer_pos) 
	    {
	    	c = buffer[buffer_pos];
	        // Se il carattere e' alfanumerico, si aggiunge al token
	        if (is_alphanumeric[c]) token[currentLen++] = tolower_table[c];

	        // Se il carattere e' un apostrofo, si controlla
	        // se fa parte di una forma contratta, come "don't"
	        else if (c == '\'') {
	            size_t j;

	            // Si cerca la forma contratta
	            for (j = 0; contractions[j].length != -1 && strncmp(token, contractions[j].string, contractions[j].length); ++j);

	            // Se esiste, la si aggiunge
	            if (contractions[j].length > 0)
	                token[currentLen++] = tolower_table[c];
	        }

	        // ...
	        else if (currentLen > 0 && token[currentLen - 1]) {
	            token[currentLen] = '\0';
	            if (*token)
                	token_insert(&tokens, token_init(token, arena));
	            currentLen = 0;
	        }
	    }
	}
	
    return tokens;
}
