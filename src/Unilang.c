/**
 * Unilang.c
 * Copyright (C) 2024 Paul Passeron
 * UNILANG source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/string_view.h"
#include "../include/unilang_lexer.h"
#include "../include/unilang_parser.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  const char fn[] = "test.ul";

  FILE *f = fopen(fn, "r");
  if (!f) {
    printf("Error opening file\n");
  }
  string_view_t s = from_file(f);
  fclose(f);
  lexer_t l = new_unilang_lexer();
  l.remaining = s;
  l.current_loc = (location_t){fn, 1, 1, false};

  while (!is_next(&l)) {
    token_t tok = next(&l);
    if (is_error_tok(tok))
      break;
    print_location_t(stdout, tok.location);
    printf(" " SF "\n", SA(tok.lexeme));
  }

  init_parsers();

  free(s.contents);
  free(l.rules.data);

  return 0;
}
