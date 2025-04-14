/**
 * unilang_lexer.h
 * Copyright (C) 2024 Paul Passeron
 * UNILANG_LEXER header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#ifndef UNILANG_LEXER_H
#define UNILANG_LEXER_H

#include "lexer.h"

typedef enum token_kind_t {
  DIRECTIVE,
  STRLIT,
  KEY_LET,
  KEY_IF,
  KEY_ELSE,
  KEY_WHILE,
  KEY_LOOP,
  KEY_RETURN,
  KEY_DEFER,
  KEY_SUM,
  KEY_CLASS,
  MULT,
  PLUS,
  DIV,
  MODULO,
  LEQ,
  LT,
  GEQ,
  GT,
  ACCESS,
  COLON,
  SEMICOLON,
  BIG_ARROW,
  EQ,
  DIFF,
  AND,
  BIT_AND,
  OR,
  BIT_OR,
  BIT_XOR,
  NOT,
  OPEN_PAR,
  CLOSE_PAR,
  OPEN_SQR,
  CLOSE_SQR,
  OPEN_BRA,
  CLOSE_BRA,
  DOT,
  SMALL_ARR,
  CHARLIT,
  INTLIT,
  FLOATLIT,
  MINUS,
  COMMA,
  IDENTIFIER,
  DEREF
} token_kind_t;

lexer_t new_unilang_lexer();
const char *human_token_kind(int kind);
void dump_token(token_t token);
void fdump_token(FILE *f, token_t token);

#endif // UNILANG_LEXER_H
