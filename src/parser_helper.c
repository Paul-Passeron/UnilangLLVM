/**
 * parser_helper.c
 * Copyright (C) 2024 Paul Passeron
 * PARSER_HELPER source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/parser_helper.h"
#include "../include/unilang_lexer.h"
#include "../include/unilang_parser.h"

ast_t *new_binop(token_t tok, ast_t *lhs, ast_t *rhs);

int get_precedence_aux(int kind) {
  switch (kind) {
  case MULT:
  case DIV:
  case MODULO:
    return 2;
  case PLUS:
    return 4;
  case MINUS:
    return 3;
  case GT:
  case LT:
  case LEQ:
  case GEQ:
    return 6;
  case EQ:
  case DIFF:
    return 7;
  case BIT_AND:
    return 8;
  case BIT_XOR:
    return 9;
  case BIT_OR:
    return 10;
  case AND:
    return 11;
  case OR:
    return 12;
  default:
    return -1;
  }
}

int get_precedence(int kind) {
  const int res = get_precedence_aux(kind);
  if (res == -1)
    return -1;
  return 13 - res;
}

bool is_kind_op(const int kind) { return get_precedence(kind) != -1; }

ast_t *parse_expression_aux(lexer_t *l, int min_precedence) {
  int worked = 0;
  ast_t *left = parse_leaf(l, &worked);
  if (!worked) {
    return NULL;
  }
  token_t tok = peek_token(l);
  if (is_error_tok(tok)) {
    return NULL;
  }
  while (true) {
    ast_t *node = parse_increasing_precedence(l, left, min_precedence);
    if (!node) {
      return NULL;
    }
    if (node == left)
      break;
    left = node;
  }
  return left;
}

ast_t *parse_increasing_precedence(lexer_t *l, ast_t *left,
                                   int min_precedence) {
  token_t tok = peek_token(l);
  if (is_error_tok(tok)) {
    return NULL;
  }
  if (!is_kind_op(tok.kind)) {
    return left;
  }
  int next_precedence = get_precedence(tok.kind);
  if (next_precedence < min_precedence) {
    return left;
  }
  next(l);
  ast_t *right = parse_expression_aux(l, next_precedence);
  if (right == NULL) {
    return NULL;
  }
  return new_binop(tok, left, right);
}
