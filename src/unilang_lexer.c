/**
 * unilang_lexer.c
 * Copyright (C) 2024 Paul Passeron
 * UNILANG_LEXER source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/unilang_lexer.h"

lexer_t new_unilang_lexer() {
  lexer_t l = {0};
  l.rules = new_rules();
  add_rule_to_lexer(&l, SV("let"), KEY_LET);
  add_rule_to_lexer(&l, SV("let"), KEY_LET);
  add_rule_to_lexer(&l, SV("if"), KEY_IF);
  add_rule_to_lexer(&l, SV("else"), KEY_ELSE);
  add_rule_to_lexer(&l, SV("while"), KEY_WHILE);
  add_rule_to_lexer(&l, SV("loop"), KEY_LOOP);
  add_rule_to_lexer(&l, SV("return"), KEY_RETURN);
  add_rule_to_lexer(&l, SV("defer"), KEY_DEFER);
  add_rule_to_lexer(&l, SV("sum"), KEY_SUM);
  add_rule_to_lexer(&l, SV("class"), KEY_CLASS);
  add_rule_to_lexer(&l, SV("\'\\\\000\'"), CHARLIT);
  add_rule_to_lexer(&l, SV("\'\\\\?\'"), CHARLIT);
  add_rule_to_lexer(&l, SV("\'?\'"), CHARLIT);
  add_rule_to_lexer(&l, SV("\"*\""), STRLIT);
  add_bad_rule_to_lexer(&l, SV("/\\*"), SV("Unmatched multi-line comment."));
  add_bad_rule_to_lexer(&l, SV("\""), SV("Unmatched string literal start."));
  add_bad_rule_to_lexer(&l, SV("\'"), SV("Unmatched char literal start."));
  add_rule_to_lexer(&l, SV("@[a-zA-Z_-_]([a-zA-Z_-_0-9])"), DIRECTIVE);
  add_rule_to_lexer(&l, SV("\\*"), MULT);
  add_rule_to_lexer(&l, SV("+"), PLUS);
  add_rule_to_lexer(&l, SV("/"), DIV);
  add_rule_to_lexer(&l, SV("%"), MODULO);
  add_rule_to_lexer(&l, SV("<="), LEQ);
  add_rule_to_lexer(&l, SV("<"), LT);
  add_rule_to_lexer(&l, SV(">="), GEQ);
  add_rule_to_lexer(&l, SV(">"), GT);
  add_rule_to_lexer(&l, SV("::"), ACCESS);
  add_rule_to_lexer(&l, SV(";"), SEMICOLON);
  add_rule_to_lexer(&l, SV(":"), COLON);
  add_rule_to_lexer(&l, SV("=>"), BIG_ARROW);
  add_rule_to_lexer(&l, SV("="), EQ);
  add_rule_to_lexer(&l, SV("!="), DIFF);
  add_rule_to_lexer(&l, SV("&&"), AND);
  add_rule_to_lexer(&l, SV("&"), BIT_AND);
  add_rule_to_lexer(&l, SV("||"), OR);
  add_rule_to_lexer(&l, SV("|"), BIT_OR);
  add_rule_to_lexer(&l, SV("^"), BIT_XOR);
  add_rule_to_lexer(&l, SV("!"), NOT);
  add_rule_to_lexer(&l, SV("\\("), OPEN_PAR);
  add_rule_to_lexer(&l, SV("\\)"), CLOSE_PAR);
  add_rule_to_lexer(&l, SV("\\["), OPEN_SQR);
  add_rule_to_lexer(&l, SV("\\]"), CLOSE_SQR);
  add_rule_to_lexer(&l, SV("{"), OPEN_BRA);
  add_rule_to_lexer(&l, SV("}"), CLOSE_BRA);
  add_rule_to_lexer(&l, SV("."), DOT);
  add_rule_to_lexer(&l, SV("->"), SMALL_ARR);
  add_rule_to_lexer(&l, SV("0b[0-1]([0-1])"), INTLIT);
  add_rule_to_lexer(&l, SV("0x[0-9a-fA-F]([0-9a-fA-F])"), INTLIT);
  add_rule_to_lexer(&l, SV("[0-9]([0-9]).[0-9]([0-9])f"), FLOATLIT);
  add_rule_to_lexer(&l, SV("[0-9]([0-9]).[0-9]([0-9])"), FLOATLIT);
  add_rule_to_lexer(&l, SV("[0-9]([0-9])f"), FLOATLIT);
  add_rule_to_lexer(&l, SV("[0-9]([0-9])"), INTLIT);
  add_rule_to_lexer(&l, SV("-"), MINUS);
  add_rule_to_lexer(&l, SV(","), COMMA);
  add_rule_to_lexer(&l, SV("[a-zA-Z_-_]([a-zA-Z_-_0-9])"), IDENTIFIER);
  add_skip_rule_to_lexer(&l, SV(" "));
  add_skip_rule_to_lexer(&l, SV("\n"));
  add_skip_rule_to_lexer(&l, SV("\t"));
  add_skip_rule_to_lexer(&l, SV("\b"));
  add_skip_rule_to_lexer(&l, SV("/\\**\\*/"));

  return l;
}

// File created by the new_file tool !
