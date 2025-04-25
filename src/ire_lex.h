#ifndef __IRE_LEX_H__
#define __IRE_LEX_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

const char* const ireX_tokens[] = {
    "search",
    "or", "and", "not",
    "(", ")", ":", ",", "|>",
    "<string>", "<number>", "<identifier>", "<eos>"
};

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
    char str[256];
    int32_t num;
} ire_SemInfo;

typedef struct {
    TokenKind kind;
    ire_SemInfo seminfo;
} ire_Token;

typedef struct {
    char* buff;
    size_t read;
    char curr;
} ire_LexState;

TokenKind llex(ire_LexState* ls, ire_SemInfo* seminfo);

#endif // __IRE_LEX_H__
