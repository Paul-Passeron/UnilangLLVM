#ifndef PPARSER_H
#define PPARSER_H
#include "lexer.h"
#include <stdlib.h>
// PREAMBULE:

#include "ast.h"
#include "parser_helper.h"
#include "unilang_lexer.h"
#include <stdint.h>

token_t *parse_token_lexeme(lexer_t *l, int *worked, string_view_t lexeme);
token_t *parse_token_kind(lexer_t *l, int *worked, int kind);
// RULE identifier
void *parse_identifier(lexer_t *l, int *worked);

// RULE intlit
void *parse_intlit(lexer_t *l, int *worked);

// RULE floatlit
void *parse_floatlit(lexer_t *l, int *worked);

// RULE charlit
void *parse_charlit(lexer_t *l, int *worked);

// RULE stringlit
void *parse_stringlit(lexer_t *l, int *worked);

// RULE boollit
void *parse_boollit(lexer_t *l, int *worked);

// RULE literal
void *parse_literal(lexer_t *l, int *worked);

// RULE param
void *parse_param(lexer_t *l, int *worked);

// RULE arglist
void *parse_arglist(lexer_t *l, int *worked);

// RULE funcall
void *parse_funcall(lexer_t *l, int *worked);

// RULE funcallargs
void *parse_funcallargs(lexer_t *l, int *worked);

// RULE leaf
void *parse_leaf(lexer_t *l, int *worked);

// RULE expr
void *parse_expr(lexer_t *l, int *worked);

// RULE stmt
void *parse_stmt(lexer_t *l, int *worked);

// RULE decl
void *parse_decl(lexer_t *l, int *worked);

// RULE binop
void *parse_binop(lexer_t *l, int *worked);

// RULE paren
void *parse_paren(lexer_t *l, int *worked);

// RULE starlist
void *parse_starlist(lexer_t *l, int *worked);

// RULE type
void *parse_type(lexer_t *l, int *worked);

// RULE unary
void *parse_unary(lexer_t *l, int *worked);

// RULE stmt_list
void *parse_stmt_list(lexer_t *l, int *worked);

// RULE compound
void *parse_compound(lexer_t *l, int *worked);

// RULE program_list
void *parse_program_list(lexer_t *l, int *worked);

// RULE program
void *parse_program(lexer_t *l, int *worked);

// RULE fundef_letless
void *parse_fundef_letless(lexer_t *l, int *worked);

// RULE fundef
void *parse_fundef(lexer_t *l, int *worked);

// RULE uop
void *parse_uop(lexer_t *l, int *worked);

// RULE vardef
void *parse_vardef(lexer_t *l, int *worked);

// RULE ct_cte
void *parse_ct_cte(lexer_t *l, int *worked);

// RULE access_spec
void *parse_access_spec(lexer_t *l, int *worked);

// RULE abstract_opt
void *parse_abstract_opt(lexer_t *l, int *worked);

// RULE method_decl
void *parse_method_decl(lexer_t *l, int *worked);

#endif // PPARSER_H
