#include "ire_lex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define is_newline(ls) (ls->curr == '\r' || ls->curr == '\n')
#define next(ls) (ls->curr = ls->buff[++ls->read])
#define raise_err(...) \
do { \
    fprintf(stderr, "ERROR: " __VA_ARGS__); \
    exit(EXIT_FAILURE); \
} while (0) \

static void skip_newline_seq(ire_LexState* ls)
{
    char old = ls->curr;
    next(ls);
    if (is_newline(ls) && ls->curr != old)  
        next(ls);
}

static void read_string(ire_LexState *ls, ire_SemInfo* seminfo)
{
    next(ls);
    size_t i = 0;
    while (ls->curr != '"') {
        switch (ls->curr) {
            case '\0': {
                raise_err("UNEXPECTED END OF STRING\n");
                break;  
            }          
        }
        seminfo->str[i++] = ls->curr;
        next(ls);
    }
    seminfo->str[i] = '\0';
    next(ls);
}

static void read_number(ire_LexState *ls, ire_SemInfo* seminfo)
{
    seminfo->num = 0;
    while (isdigit(ls->curr)) {
        seminfo->num = (seminfo->num * 10) + (int32_t)(ls->curr - '0');
        next(ls);
    }
}

TokenKind llex(ire_LexState* ls, ire_SemInfo* seminfo)
{
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
                read_string(ls, seminfo);
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
                    } while (isalpha(ls->curr));
                    seminfo->str[i] = '\0';

                    if (!strcmp(seminfo->str, "or")) return TK_OR;
                    else if (!strcmp(seminfo->str, "and")) return TK_AND;
                    else if (!strcmp(seminfo->str, "not")) return TK_NOT;
                    else if (!strcmp(seminfo->str, "search")) return TK_SEARCH;
                    else return TK_IDENTIFIER;

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
                        }
                    }

                    raise_err("UNEXPECTED SYMBOL\n");
                }
            }
        }
    }
}