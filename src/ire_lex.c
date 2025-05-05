#include "ire_lex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define is_newline(ls) (ls->curr == '\r' || ls->curr == '\n')

// Nota: il non bound checking e' voluto
#define next(ls) (ls->curr = ls->buff[++ls->read])

#define raise_err(...) \
do { \
    fprintf(stderr, "ERROR: " __VA_ARGS__); \
} while (0) \

static void skip_newline_seq(ire_LexState* ls)
{
    char old = ls->curr;
    next(ls);
    if (is_newline(ls) && ls->curr != old)  
        next(ls);
}

#define E_STR_LNG -1
#define E_STR_UNEXP_END -2
#define E_ID_UNEXP_END -3
#define E_UNEXP_SYM -4

static int32_t read_string(ire_LexState* ls, ire_SemInfo* seminfo)
{
    next(ls);
    size_t i = 0;
    while (ls->curr != '"' && i < STRING_LEN) {
		if (ls->curr == '\0' || is_newline(ls)) 
			return E_STR_UNEXP_END;
        seminfo->str[i++] = ls->curr;
        next(ls);
    }
    if (i < STRING_LEN) {
	    seminfo->str[i] = '\0';
	    next(ls);	
    } else {
    	return E_STR_LNG;
    }
    return 0;
}

// Nota: solo numeri naturali (0 e' un naturale, coglione) in base 10
static void read_number(ire_LexState *ls, ire_SemInfo* seminfo)
{
    seminfo->num = 0;
    while (isdigit(ls->curr)) {
        seminfo->num = (seminfo->num * 10) + (int32_t) (ls->curr - '0');
        next(ls);
    }
}

TokenKind llex(ire_LexState* ls, ire_SemInfo* seminfo)
{
	ls->errcheck = 0;
    for (;;) {
        switch (ls->curr) {
            case '\0': {
                return TK_EOS;
            }
            case '\n': case '\r': {
                skip_newline_seq(ls);
                break;
            }
            case ' ': case '\f': case '\t': case '\v': {
                next(ls);
                break;
            }
            case '"': {
				ls->errcheck = read_string(ls, seminfo);
                return TK_STRING;
            }
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9': {
                read_number(ls, seminfo);
                return TK_NUMBER;
            }
            default: {
                if (isalpha(ls->curr)) {

                    size_t i = 0;
                    do {
                        seminfo->str[i++] = ls->curr;
                        next(ls);
                    } while (isalpha(ls->curr) && i < STRING_LEN);

                    if (i < STRING_LEN) {
	                    seminfo->str[i] = '\0';
	                    if (!strcmp(seminfo->str, "or")) return TK_OR;
	                    else if (!strcmp(seminfo->str, "and")) return TK_AND;
	                    else if (!strcmp(seminfo->str, "not")) return TK_NOT;
	                    else if (!strcmp(seminfo->str, "search")) return TK_SEARCH;
						// fallthrough...
                    } else {
                   		ls->errcheck = E_ID_UNEXP_END;
                    }

                    return TK_IDENTIFIER;

                }  else {
                    char c = ls->curr;
                    next(ls);

                    switch (c) {
                        case '(': return TK_L_PAREN;
                        case ')': return TK_R_PAREN;
                        case ':': return TK_COLON;
                        case ',': return TK_COMMA;
                        case '|': {
                            if (ls->curr == '>') {
                                next(ls);
                                return TK_PIPE;
                            }
                            // fallthrough...
                        }
                    }

                    ls->errcheck = E_UNEXP_SYM;
                }
            }
        }
    }
}
