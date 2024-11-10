/**
 * Unilang.c
 * Copyright (C) 2024 Paul Passeron
 * UNILANG source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/lexer.h"
#include "../include/string_view.h"

int main(void) {
  string_view_t s = SV("@include \"io.ul\"\nlet main(): void => "
                       "{\n\tprintln(\"Hello, World !\");\n}\n");
  lexer_t l = {{"hello_world.ul", 1, 1, false}, s, new_rules()};

  add_bad_rule_to_lexer(&l, SV("*"), SV("Everything must burn"));

  token_t tok = next(&l);
  (void)tok;
  return 0;
}
