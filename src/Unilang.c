/**
 * Unilang.c
 * Copyright (C) 2024 Paul Passeron
 * UNILANG source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/lexer.h"
#include "../include/string_view.h"
#include <stdlib.h>

int main(void) {
  char input[] = "@include \"io.ul\"\nlet main(): void => "
                 "{\n    println(\"Hello, World !\");\n}\n";
  string_view_t s = SV(input);
  printf("INPUT IS \n" SF, SA(s));
  //   lexer_t l = {{"hello_world.ul", 1, 1, false}, s, new_rules()};
  lexer_t l = new_unilang_lexer();
  l.remaining = s;
  l.current_loc = (location_t){"hello_world.ul", 1, 1, false};

  while (!is_next(&l)) {
    token_t tok = next(&l);
    if (is_error_tok(tok))
      break;
    printf("Token lexeme is " SF "\n", SA(tok.lexeme));
  }

  free(l.rules.data);
  return 0;
}
