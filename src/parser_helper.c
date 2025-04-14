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
  case DEREF:
    return 0;
  case ACCESS:
    return 1;
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
  case OPEN_PAR:
  case OPEN_SQR:
    return 13;
  default:
    return -1;
  }
}

int get_precedence(int kind) {
  const int res = get_precedence_aux(kind);
  if (res == -1)
    return -1;
  return 14 - res;
}

bool is_prefix(token_kind_t kind) {
  switch (kind) {
  case PLUS:
  case MINUS:
  case DEREF:
  case NOT:
    return true;
  default:
    return false;
  }
}

bool is_kind_op(const int kind) {
  return get_precedence(kind) != -1 && kind != OPEN_PAR && kind != OPEN_SQR &&
         kind != DEREF;
}

bool is_postfix(lexer_t *l) {
  token_t t = peek_token(l);
  if (is_error_tok(t))
    return 0;
  switch (t.kind) {
  case OPEN_PAR:
  case OPEN_SQR:
    return true;
  default:
    return false;
  }
}

ast_t *parse_funcall(lexer_t *l, ast_t *called) {
  int w;
  token_t tok = peek_token(l);
  ast_t **elems;
  size_t count = 0;
  if (tok.kind == CLOSE_PAR) {
    elems = malloc(sizeof(ast_t *));
    *elems = NULL;
  } else {
    elems = parse_funcallargs(l, &w);
    while (elems[count]) {
      count++;
    }
    elems = realloc(elems, sizeof(ast_t *) * count);
  }
  tok = next(l);
  if (tok.kind != CLOSE_PAR) {
    return NULL;
  }
  return new_funcall(called, count, elems);
}

ast_t *parse_prefix(lexer_t *l) {
  token_t tok = peek_token(l);
  if (!is_prefix(tok.kind)) {
    return NULL;
  }
  next(l);
  ast_t *operand = parse_expression_aux(l, get_precedence(tok.kind));
  if (!operand) {
    return NULL;
  }

  return new_unop(tok, operand);
}

ast_t *parse_postfix(lexer_t *l, ast_t *left) {
  token_t tok = next(l);
  if (is_error_tok(tok))
    return left;
  if (tok.kind == OPEN_PAR) {
    return parse_funcall(l, left);
  } else if (tok.kind == OPEN_SQR) {
    int w;
    ast_t *expr = parse_expr(l, &w);
    if (!w)
      return NULL;
    tok = next(l);
    if (tok.kind != CLOSE_SQR) {
      return NULL;
    }
    return new_index(left, expr);
  }
  return left;
}

ast_t *parse_expression_aux(lexer_t *l, int min_precedence) {
  int worked = 0;
  ast_t *left;

  token_t tok = peek_token(l);
  if (is_prefix(tok.kind)) {
    left = parse_prefix(l);
    if (!left) {
      worked = 0;
      return NULL; // Error in parsing prefix expression
    }
  } else {
    left = parse_leaf(l, &worked);
    if (!worked) {
      return NULL; // Error in parsing leaf expression
    }
  }
  tok = peek_token(l);
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
  while (is_postfix(l)) {
    int postfix_precedence = get_precedence(peek_token(l).kind);
    if (postfix_precedence < min_precedence) {
      break;
    }
    left = parse_postfix(l, left);
  }
  if (!is_kind_op(tok.kind)) {
    return left;
  }
  int next_precedence = get_precedence(tok.kind);
  if (next_precedence <= min_precedence) {
    return left;
  }
  next(l);
  ast_t *right = parse_expression_aux(l, next_precedence);
  if (right == NULL) {
    return NULL;
  }
  return new_binop(tok, left, right);
}
