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

const char *human_token_kind(int kind) {
  switch (kind) {
  case DIRECTIVE:
    return "DIRECTIVE";
  case STRLIT:
    return "STRLIT";
  case KEY_LET:
    return "KEY_LET";
  case KEY_IF:
    return "KEY_IF";
  case KEY_ELSE:
    return "KEY_ELSE";
  case KEY_WHILE:
    return "KEY_WHILE";
  case KEY_LOOP:
    return "KEY_LOOP";
  case KEY_RETURN:
    return "KEY_RETURN";
  case KEY_DEFER:
    return "KEY_DEFER";
  case KEY_SUM:
    return "KEY_SUM";
  case KEY_CLASS:
    return "KEY_CLASS";
  case MULT:
    return "MULT";
  case PLUS:
    return "PLUS";
  case DIV:
    return "DIV";
  case MODULO:
    return "MODULO";
  case LEQ:
    return "LEQ";
  case LT:
    return "LT";
  case GEQ:
    return "GEQ";
  case GT:
    return "GT";
  case ACCESS:
    return "ACCESS";
  case COLON:
    return "COLON";
  case SEMICOLON:
    return "SEMICOLON";
  case BIG_ARROW:
    return "BIG_ARROW";
  case EQ:
    return "EQ";
  case DIFF:
    return "DIFF";
  case AND:
    return "AND";
  case BIT_AND:
    return "BIT_AND";
  case OR:
    return "OR";
  case BIT_OR:
    return "BIT_OR";
  case BIT_XOR:
    return "BIT_XOR";
  case NOT:
    return "NOT";
  case OPEN_PAR:
    return "OPEN_PAR";
  case CLOSE_PAR:
    return "CLOSE_PAR";
  case OPEN_SQR:
    return "OPEN_SQR";
  case CLOSE_SQR:
    return "CLOSE_SQR";
  case OPEN_BRA:
    return "OPEN_BRA";
  case CLOSE_BRA:
    return "CLOSE_BRA";
  case DOT:
    return "DOT";
  case SMALL_ARR:
    return "SMALL_ARR";
  case CHARLIT:
    return "CHARLIT";
  case INTLIT:
    return "INTLIT";
  case FLOATLIT:
    return "FLOATLIT";
  case MINUS:
    return "MINUS";
  case COMMA:
    return "COMMA";
  case IDENTIFIER:
    return "IDENTIFIER";
  default:
    return "ERROR";
  }
}

void dump_token(token_t token) {
  print_location_t(stdout, token.location);
  printf("\'" SF "\' %s", SA(token.lexeme), human_token_kind(token.kind));
}

void fdump_token(FILE *f, token_t token) {
  print_location_t(f, token.location);
  fprintf(f, "\'" SF "\' %s", SA(token.lexeme), human_token_kind(token.kind));
}
