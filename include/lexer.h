/**
 * lexer.h
 * Copyright (C) 2024 Paul Passeron
 * LEXER header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#ifndef LEXER_H
#define LEXER_H

#include "string_view.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct location_t {
  const char *filename;
  int line;
  int col;
  bool is_expanded;
} location_t;

typedef struct token_t {
  location_t location;
  string_view_t lexeme;
} token_t;

typedef enum lexer_rule_kind_t {
  GOOD,
  BAD,
  SKIP,
} lexer_rule_kind_t;

typedef union lexer_rule_as_t {
  int good;
  string_view_t error;
} lexer_rule_as_t;

typedef struct lexer_rule_t {
  lexer_rule_kind_t kind;
  lexer_rule_as_t as;
} lexer_rule_t;

typedef struct lexer_rules_t {
  lexer_rule_t *data;
  size_t count;
  size_t size;
} lexer_rules_t;

typedef struct lexer_t {
  location_t current_loc;
  string_view_t remaining;
  lexer_rules_t rules;
} lexer_t;

void print_location_t(FILE *f, location_t loc);

#endif // LEXER_H
