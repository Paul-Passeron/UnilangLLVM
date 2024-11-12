/**
 * unilang_parser.c
 * Copyright (C) 2024 Paul Passeron
 * UNILANG_PARSER source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/unilang_parser.h"
#include "../include/ast.h"
#include "../include/defs.h"

parser_t let_parser;
parser_t identifier_parser;

token_t tok_lexeme_rule(lexer_t *l, string_view_t lexeme) {
  if (!is_next(l)) {
    // print error ?
    return error_token();
  }
  token_t tok = next(l);
  if (sv_eq(tok.lexeme, lexeme)) {
    return tok;
  }
  return error_token();
}

token_t tok_kind_rule(lexer_t *l, int kind) {
  if (!is_next(l)) {
    // print error ?
    return error_token();
  }
  token_t tok = next(l);
  if (tok.kind == kind) {
    return tok;
  }
  return error_token();
}

void *let_parser_rule(lexer_t *l, parser_t self) {
  UNUSED(self);
  // its return value should not used except when checking it against NULL to
  // see if the rule was successful
  if (is_error_tok(tok_kind_rule(l, KEY_LET))) {
    return NULL;
  }
  return (void *)1;
}

void *identifier_parser_rule(lexer_t *l, parser_t self) {
  UNUSED(self);
  token_t res = tok_kind_rule(l, IDENTIFIER);
  if (is_error_tok(res)) {
    return NULL;
  }
  return new_identifier(res);
}

void init_parsers(void) {
  let_parser = (parser_t){.kind = UNIT,
                          .children = (parsers_t){NULL, 0},
                          .rule = let_parser_rule,
                          .error_message = SV("Expected \'let\' keyword.")};
  identifier_parser = (parser_t){.kind = UNIT,
                                 .children = (parsers_t){NULL, 0},
                                 .rule = identifier_parser_rule,
                                 .error_message = SV("Expected identifier.")};
}
