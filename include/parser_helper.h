/**
 * parser_helper.h
 * Copyright (C) 2024 Paul Passeron
 * PARSER_HELPER header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

// File created by the new_file tool !

#ifndef PARSER_HELPER_H
#define PARSER_HELPER_H

#include "ast.h"

ast_t *parse_expression_aux(lexer_t *l, int min_precedence);

ast_t *parse_increasing_precedence(lexer_t *l, ast_t *left, int min_precedence);

int get_precedence_aux(int kind);
int get_precedence(int kind);

bool is_kind_op(const int kind);

#endif // PARSER_HELPER_H
