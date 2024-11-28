/**
 * ast.c
 * Copyright (C) 2024 Paul Passeron
 * AST source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/ast.h"
#include <stdio.h>
#include <stdlib.h>

ast_t *new_identifier(token_t tok) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_IDENTIFIER;
  res->as.identifier = (ast_identifier_t){tok};
  return res;
}

void free_identifier(ast_t *iden) {
  // just doing that for the moment
  if (iden->kind != AST_IDENTIFIER)
    return; // not an identifier
  free(iden);
}

ast_t *new_floatlit(token_t tok) {
  ast_t *res = new_identifier(tok);
  res->kind = AST_FLOATLIT;
  return res;
}
void free_floatlit(ast_t *iden) { free_identifier(iden); }

ast_t *new_charlit(token_t tok) {
  ast_t *res = new_identifier(tok);
  res->kind = AST_CHARLIT;
  return res;
}
void free_charlit(ast_t *iden) { free_identifier(iden); }

ast_t *new_stringlit(token_t tok) {
  ast_t *res = new_identifier(tok);
  res->kind = AST_STRINGLIT;
  return res;
}
void free_stringlit(ast_t *iden) { free_identifier(iden); }

ast_t *new_intlit(token_t tok) {
  ast_t *res = new_identifier(tok);
  res->kind = AST_INTLIT;
  return res;
}
void free_intlit(ast_t *iden) { free_identifier(iden); }

ast_t *new_boollit(int val) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_BOOLLIT;
  res->as.boollit = (ast_boollit_t){val};
  return res;
}
void free_boollit(ast_t *iden) { free(iden); }

#define print_token(tok) printf(SF, SA(tok.lexeme))

void dump_ast(ast_t *ast) {
  if (ast == NULL) {
    printf("null");
    return;
  }
  printf("{\n");
  printf("  \"kind\": ");
  switch (ast->kind) {
  case AST_IDENTIFIER:
    printf("\"AST_IDENTIFIER\",\n");
    printf("  \"value\": \"");
    print_token(ast->as.identifier.tok);
    printf("\"\n");
    break;
  case AST_INTLIT:
    printf("\"AST_INTLIT\",\n");
    printf("  \"value\": \"" SF "\"\n",
           SA(ast->as.intlit.tok.lexeme)); // Assuming tok.value is a string
                                           // representation of the integer
    break;
  case AST_FLOATLIT:
    printf("\"AST_FLOATLIT\",\n");
    printf("  \"value\": \"" SF "\"\n", SA(ast->as.floatlit.tok.lexeme));
    break;
  case AST_CHARLIT:
    printf("\"AST_CHARLIT\",\n");
    printf("  \"value\": \"" SF "\"\n", SA(ast->as.charlit.tok.lexeme));
    break;
  case AST_STRINGLIT:
    printf("\"AST_STRINGLIT\",\n");
    printf("  \"value\": " SF "\n", SA(ast->as.stringlit.tok.lexeme));
    break;
  case AST_BOOLLIT:
    printf("\"AST_BOOLLIT\",\n");
    printf("  \"value\": %s\n", ast->as.boollit.val ? "true" : "false");
    break;
  case AST_FUNDEF:
    printf("\"AST_FUNDEF\",\n");
    printf("  \"name\": \"");
    print_token(ast->as.fundef.name);
    printf("\",\n  \"param_count\": %zu,\n", ast->as.fundef.param_count);

    printf("  \"param_types\": [");
    for (size_t i = 0; i < ast->as.fundef.param_count; ++i) {
      if (i > 0) {
        printf(", ");
      }
      printf("\"" SF "\"",
             SA(ast->as.fundef.param_types[i]->as.identifier.tok.lexeme));
    }
    printf("],\n");

    printf("  \"param_names\": [");
    for (size_t i = 0; i < ast->as.fundef.param_count; ++i) {
      if (i > 0) {
        printf(", ");
      }
      printf("\"");
      print_token(
          ast->as.fundef.param_names[i]); // Assuming print_token handles
                                          // token_t appropriately
      printf("\"");
    }
    printf("],\n");
    printf("\"body\": ");
    dump_ast(ast->as.fundef.body);
    break;
  case AST_COMPOUND: {
    printf("\"AST_COMPOUND\", \n");
    printf("\"content\": [\n");
    for (size_t i = 0; i < ast->as.compound.elem_count; ++i) {
      if (i > 0)
        printf(",\n");
      dump_ast(ast->as.compound.elems[i]);
    }
    printf("]\n");
  } break;
  case AST_FUNCALL: {
    printf("\"AST_FUNCALL\",");
    printf("\"called\": \n\"");
    print_token(ast->as.funcall.called);
    printf("\", \"args\": [");
    for (size_t i = 0; i < ast->as.funcall.arg_count; i++) {
      if (i > 0) {
        printf(",\n");
      }
      dump_ast(ast->as.funcall.args[i]);
    }
    printf("]\n");
  } break;
  case AST_BINOP: {
    printf("\"AST_BINOP\",\n");
    printf("\"operator\": \"");
    print_token(ast->as.binop.op);
    printf("\",\n\"lhs\":");
    dump_ast(ast->as.binop.lhs);
    printf(",\n\"rhs\":");
    dump_ast(ast->as.binop.rhs);
  } break;
  default:
    printf("\"UNKNOWN_KIND\"\n");
    break;
  }
  printf("}\n");
}

void free_fundef(ast_t *ast) {
  // TODO
  return;
}

void free_ast(ast_t *ast) {
  if (ast == NULL)
    return;
  switch (ast->kind) {
  case AST_IDENTIFIER:
    free_identifier(ast);
    break;
  case AST_INTLIT:
    free_intlit(ast);
    break;
  case AST_STRINGLIT:
    free_stringlit(ast);
    break;
  case AST_CHARLIT:
    free_charlit(ast);
    break;
  case AST_FLOATLIT:
    free_floatlit(ast);
    break;
  case AST_BOOLLIT:
    free_boollit(ast);
    break;
  case AST_FUNDEF:
    free_fundef(ast);
    break;
  case AST_COMPOUND:
    // TODO
    break;
  case AST_FUNCALL: {
    printf("TODO AST FUNCALL\n");
    exit(1);
  } break;
  case AST_UNOP: {
    printf("TODO AST_UNOP");
    exit(1);
  } break;
  case AST_BINOP: {
    printf("TODO AST_BINOP");
    exit(1);
  } break;
  }
}

ast_t *new_fundef_from_parser(token_t id, tmp_param_t **arglist,
                              ast_t **stmt_list) {
  size_t i = 0;
  while (arglist[i]) {
    i++;
  }
  ast_t **param_types = malloc(sizeof(ast_t *) * i);
  token_t *param_names = malloc(sizeof(token_t) * i);
  for (size_t j = 0; j < i; j++) {
    param_types[j] = arglist[j]->type;
    param_names[j] = arglist[j]->name->as.identifier.tok;
    free_ast(arglist[j]->name);
  }
  free(arglist);
  return new_fundef(id, i, param_types, param_names, new_compound(stmt_list));
}

tmp_param_t *new_param(ast_t *id, ast_t *type) {
  tmp_param_t *res = malloc(sizeof(tmp_param_t));
  res->type = type;
  res->name = id;
  return res;
}

ast_t *new_fundef(token_t name, size_t param_count, ast_t **param_types,
                  token_t *param_names, ast_t *body) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_FUNDEF;
  res->as.fundef.name = name;
  res->as.fundef.param_count = param_count;
  res->as.fundef.param_types = param_types;
  res->as.fundef.param_names = param_names;
  res->as.fundef.body = body;
  return res;
}

ast_t *new_compound(ast_t **elems) {
  size_t count = 0;
  while (elems[count]) {
    count++;
  }
  ast_t *res = malloc(sizeof(ast_t));
  ast_t **new_elems = realloc(elems, sizeof(ast_t *) * count);
  res->kind = AST_COMPOUND;
  res->as.compound = (ast_compound_t){count, new_elems};
  return res;
}

void free_compound(ast_t *ast) {
  // TODO
}

ast_t *new_funcall(token_t called, size_t arg_count, ast_t **args) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_FUNCALL;
  res->as.funcall = (ast_funcall_t){called, arg_count, args};
  return res;
}

ast_t *new_unop(token_t op, ast_t *operand) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_UNOP;
  res->as.unop = (ast_unop_t){op, operand};
  return res;
}
ast_t *new_binop(token_t op, ast_t *lhs, ast_t *rhs) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_BINOP;
  res->as.binop = (ast_binop_t){op, lhs, rhs};
  return res;
}