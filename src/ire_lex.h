#ifndef __IRE_LEX_H__
#define __IRE_LEX_H__

#include <stddef.h>
#include <stdint.h>

#define STRING_LEN 256

typedef enum {
	TK_SEARCH,

	TK_OR, 
	TK_AND, 
	TK_NOT, 

	TK_L_PAREN, 
	TK_R_PAREN, 
	TK_COLON,   
	TK_COMMA,   
	TK_PIPE, // |>

	TK_STRING, 
	TK_NUMBER, // numero naturale 
	TK_IDENTIFIER, 

	TK_EOS // eos = end of string
} TokenKind;

typedef union {
    char str[STRING_LEN + 1];
    int32_t num;
} ire_SemInfo;

typedef struct {
    TokenKind kind;
    ire_SemInfo seminfo;
} ire_Token;

typedef struct {
    size_t read; 
    char* buff;
    char curr;
	int32_t errcheck;
} ire_LexState;

TokenKind llex(ire_LexState* ls, ire_SemInfo* seminfo);

#endif // __IRE_LEX_H__
