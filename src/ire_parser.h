#ifndef __IRE_PARSER_H__
#define __IRE_PARSER_H__

#include <stddef.h>

typedef enum {
    AST_PIPELINE,
    AST_CALL,
    AST_SEARCH,
    AST_OR,
    AST_AND,
    AST_NOT,
    AST_TERM,
    AST_NUMBER,
    AST_STRING,
    AST_IDENTIFIER
} AST_NodeKind;

typedef struct ire_AST ire_AST;

struct ire_AST {
    AST_NodeKind kind;
    union {
        struct { ire_AST** stages; size_t count; } pipeline;
        struct { char* fname; ire_AST** argv; size_t argc; } call;
        struct { ire_AST* query; } search_call;
        struct { ire_AST* left; ire_AST* right; } binary_op;
        struct { ire_AST* expr; } unary_op;
        struct { char* value; } term;
    };
};

typedef struct { 

} ire_Parser;

ire_AST* parse(char* expression);
ire_AST* parse_statement(ire_Parser* parser);
ire_AST* parse_search_call(ire_Parser* parser);
ire_AST* parse_call(ire_Parser* parser);
ire_AST* parse_args(ire_Parser* parser);
ire_AST* parse_arg(ire_Parser* parser);
ire_AST* parse_named_arg(ire_Parser* parser);
ire_AST* parse_expr(ire_Parser* parser);
ire_AST* parse_query(ire_Parser* parser);
ire_AST* parse_or_expr(ire_Parser* parser);
ire_AST* parse_and_expr(ire_Parser* parser);
ire_AST* parse_not_expr(ire_Parser* parser);
ire_AST* parse_primary(ire_Parser* parser);

#endif // __IRE_PARSER_H__