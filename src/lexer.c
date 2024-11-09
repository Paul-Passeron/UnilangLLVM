/**
 * lexer.c
 * Copyright (C) 2024 Paul Passeron
 * LEXER source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/lexer.h"

void print_location_t(FILE *f, location_t loc) {
  fprintf(f, "%s:%d:%d: %s", loc.filename, loc.line, loc.col,
          loc.is_expanded ? "[IN MACRO EXPANSION] " : "");
}