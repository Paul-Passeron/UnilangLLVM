/**
 * Unilang.c
 * Copyright (C) 2024 Paul Passeron
 * UNILANG source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/generator.h"
#include "../include/string_view.h"
#include "../include/unilang_lexer.h"
#include "../include/unilang_parser.h"
#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage(const char *str) {
  printf("Usage: %s [options] <input file>\n", str);
}

void dump_tokens(lexer_t l) {
  lexer_t cpy = l;
  while (!is_next(&cpy)) {
    token_t tok = next(&cpy);
    if (is_error_tok(tok))
      break;
    fdump_token(stderr, tok);
    fprintf(stderr, "\n");
  }
}

void print_cmd(int argc, char **argv) {
  printf("[CMD] ");
  for (int i = 0; i < argc; i++) {
    printf("%s ", argv[i]);
  }
  printf("\n");
}

int main(int argc, char **argv) {
  print_cmd(argc, argv);
  if (argc < 2) {
    usage(argv[0]);
    return 1;
  }
  char *fn = NULL;
  char *out = NULL;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (strcmp(argv[i], "-o") == 0) {
        i++;
        if (i == argc) {
          printf("Expected file name after \'-o\' flag.\n");
          usage(argv[0]);
          return 3;
        }
        printf("TODO: output to \'%s\'\n", argv[i]);
        out = argv[i];
      }
    } else if (fn == NULL) {
      fn = argv[i];
    } else {
      printf("Can only compile one file at a time for the moment\n");
      usage(argv[0]);
      return 4;
    }
  }

  if (fn == NULL) {
    printf("Expected input file\n");
    usage(argv[0]);
    return 2;
  }

  if (out == NULL) {
    out = "a.out";
  }
  FILE *f = fopen(fn, "r");
  if (!f) {
    printf("Error opening file \'%s\': ", fn);
    fflush(stdout);
    perror("");
    printf("n");
    usage(argv[0]);
    return 5;
  }
  string_view_t s = from_file(f);
  fclose(f);
  lexer_t l = new_unilang_lexer();
  l.remaining = s;
  l.current_loc = (location_t){fn, 1, 1, false};

  // dump_tokens(l);

  int worked = 0;
  ast_t *prog = parse_program(&l, &worked);
  if (worked) {
    // dump_ast(prog);
    // printf("\n");
    // free_ast(prog);
  } else {
    printf("NOOO\n");
  }

  generator_t g;
  init(&g, "main");

  // for (size_t i = 0; i < g.nb_types; i++) {
  //   printf("%s: ", g.types[i].name);
  //   fflush(stdout);
  //   LLVMDumpType(g.types[i].type);
  //   printf("\n");
  // }

  // printf("*******************\n");
  // fflush(stdout);
  generate_program(&g, prog);

  // free(s.contents);
  // free(l.rules.data);

  char *error = NULL;
  LLVMVerifyModule(g.module, LLVMPrintMessageAction, &error);
  LLVMDisposeMessage(error);

  return 0;
}
