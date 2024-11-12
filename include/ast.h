/**
 * ast.h
 * Copyright (C) 2024 Paul Passeron
 * AST header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#ifndef AST_H
#define AST_H

#include "lexer.h"

typedef struct ast_t ast_t;

typedef enum ast_kind_t { AST_IDENTIFIER } ast_kind_t;

typedef struct ast_identifier_t {
  token_t tok;
} ast_identifier_t;

ast_t *new_identifier(token_t tok);
void free_identifier(ast_t *iden);

typedef union ast_as_t {
  ast_identifier_t identifier;
} ast_as_t;

struct ast_t {
  ast_kind_t kind;
  ast_as_t as;
};

#endif // AST_H
