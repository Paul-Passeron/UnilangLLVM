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
#include <llvm-c/Support.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
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

  int worked = 0;
  ast_t *prog = parse_program(&l, &worked);
  if (!worked) {
    printf("Parsing failed\n");
    exit(1);
  }
  dump_ast(prog);
  printf("\n");
  generator_t g;
  generator_init(&g);

  generate_program(prog);
  fflush(stdout);

  LLVMDumpModule(g.module);

  char *error = NULL;
  if (LLVMVerifyModule(g.module, LLVMPrintMessageAction, &error)) {
    printf("Error in generated LLVM code: %s\n", error);
  }
  LLVMDisposeMessage(error);

  char *triple = LLVMGetDefaultTargetTriple();
  LLVMTargetRef target;
  char *err = NULL;

  if (LLVMGetTargetFromTriple(triple, &target, &err)) {
    fprintf(stderr, "Error getting target: %s\n", err);
    LLVMDisposeMessage(err);
    return 1;
  }

  LLVMTargetMachineRef target_machine = LLVMCreateTargetMachine(
      target, triple,
      "generic", // CPU type
      "",        // Features
      LLVMCodeGenLevelDefault, LLVMRelocDefault, LLVMCodeModelDefault);

  LLVMDisposeMessage(triple);

  char *filename = "tests/output.s";
  if (LLVMTargetMachineEmitToFile(target_machine, g.module, filename,
                                  LLVMAssemblyFile, &error)) {
    fprintf(stderr, "Error emitting assembly: %s\n", error);
    LLVMDisposeMessage(error);
    return 1;
  }

  printf("\n");
  printf("\n");
  return 0;
}
