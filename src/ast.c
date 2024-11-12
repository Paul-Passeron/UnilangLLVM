/**
 * ast.c
 * Copyright (C) 2024 Paul Passeron
 * AST source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/ast.h"
#include <stdlib.h>

ast_t *new_identifier(token_t tok) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_IDENTIFIER;
  res->as.identifier = (ast_identifier_t){tok};
  return res;
}

void free_identifier(ast_t *iden) {
  // just doing that for the moment
  if (iden->kind != AST_IDENTIFIER)
    return; // not an identifier
  free(iden);
}