/**
 * unilang_parser.c
 * Copyright (C) 2024 Paul Passeron
 * UNILANG_PARSER source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/unilang_parser.h"
#include "../include/defs.h"
parser_t let_parser;

void *let_parser_rule(lexer_t *l, parser_t self) {
  UNUSED(self);
  // its return value should not used except when checking it against NULL to
  // see if the rule was successful
  if (!is_next(l)) {
    // print error ?
    return NULL;
  }
  token_t tok = next(l);
  if (!sv_eq(tok.lexeme, SV("let"))) {
    return NULL;
  }
  return (void *)1;
}

void init_parsers(void) {
  let_parser = (parser_t){.kind = UNIT,
                          .children = (parsers_t){NULL, 0},
                          .rule = let_parser_rule,
                          .error_message = SV("Expected \'let\' keyword.")};
}
