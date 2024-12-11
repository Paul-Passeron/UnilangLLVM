/**
 * generator.c
 * Copyright (C) 2024 Paul Passeron
 * GENERATOR source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/generator.h"
#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Transforms/PassBuilder.h>

#include <llvm-c/Types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void addBuiltinTypes(generator_t *g) {
  add_type(g, "int", LLVMInt32Type());
  add_type(g, "char", LLVMInt8Type());

  add_type(g, "i64", LLVMInt64Type());
  add_type(g, "i32", LLVMInt32Type());
  add_type(g, "i16", LLVMInt16Type());
  add_type(g, "i8", LLVMInt8Type());

  add_unsigned_type(g, "u64", LLVMInt64Type());
  add_unsigned_type(g, "u32", LLVMInt32Type());
  add_unsigned_type(g, "u16", LLVMInt16Type());
  add_unsigned_type(g, "u8", LLVMInt8Type());

  add_type(g, "void", LLVMVoidType());
}

void add_actual_type(generator_t *g, type_t t) {
  if (g->nb_types >= g->cap_types) {
    type_t *new_types = malloc(g->cap_types * 2 * sizeof(type_t));
    memcpy(new_types, g->types, g->cap_types * sizeof(type_t));
    free(g->types);
    g->types = new_types;
    g->cap_types *= 2;
  }
  g->types[g->nb_types++] = t;
}

void add_unsigned_type(generator_t *g, const char *name, LLVMTypeRef type) {
  type_t t = {0, 0, strdup(name), type};
  add_actual_type(g, t);
}

void init(generator_t *g, const char *module_name) {
  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
  LLVMInitializeNativeAsmParser();

  g->context = LLVMContextCreate();
  g->module = LLVMModuleCreateWithNameInContext(module_name, g->context);
  g->builder = LLVMCreateBuilderInContext(g->context);

  g->cap_types = INIT_TYPES_CAP;
  g->types = malloc(sizeof(type_t) * g->cap_types);
  g->nb_types = 0;

  addBuiltinTypes(g);
}

void add_type(generator_t *g, const char *name, LLVMTypeRef type) {
  type_t t = {0, 1, strdup(name), type};
  add_actual_type(g, t);
}
// call it only if you know for sure the type exists, it will return garbage
// otherwise
type_t get_type_from_name(generator_t *g, const char *name) {
  for (size_t i = 0; i < g->nb_types; i++) {
    if (strcmp(g->types[i].name, name) == 0) {
      return g->types[i];
    }
  }
  fflush(stdout);
  return (type_t){0};
}

type_t get_ptr_of(type_t t) {
  LLVMTypeRef ptr = LLVMPointerType(t.type, 0);
  return (type_t){1, 0, strdup("ptr"), ptr};
}

int is_type_defined(generator_t *g, const char *name) {
  for (size_t i = 0; i < g->nb_types; i++) {
    if (strcmp(g->types[i].name, name) == 0) {
      return 1;
    }
  }
  return 0;
}

type_t get_type_from_ast(generator_t *g, ast_t *type) {
  if (type->kind != AST_TYPE) {
    return (type_t){0};
  }
  if (type->as.type.ptr_n == 0) {
    token_t tok = type->as.type.name;
    char *n = strndup(tok.lexeme.contents, tok.lexeme.length);
    type_t res = (type_t){0};
    if (is_type_defined(g, n)) {
      res = get_type_from_name(g, n);
    }
    free(n);
    return res;
    return (type_t){0};
  } else {
    printf("ptrs not yet supported\n");
    exit(1);
  }
}

void generate_program(generator_t *g, ast_t *program) {
  for (size_t i = 0; i < program->as.program.elem_count; i++) {
    ast_t *decl = program->as.program.elems[i];
    if (decl->kind == AST_FUNDEF) {
      generate_function(g, decl);
    }
  }
}

void generate_function(generator_t *g, ast_t *fun) {
  ast_fundef_t f = fun->as.fundef;
  string_view_t svn = f.name.lexeme;
  char *name = strndup(svn.contents, svn.length);
  type_t return_type = get_type_from_ast(g, f.return_type);
  LLVMTypeRef r = return_type.type;
  type_t *types = malloc(sizeof(type_t) * f.param_count);
  LLVMTypeRef *param_types = malloc(sizeof(LLVMTypeRef) * f.param_count);
  for (size_t i = 0; i < f.param_count; ++i) {
    types[i] = get_type_from_ast(g, f.param_types[i]);
    param_types[i] = types[i].type;
    // LLVMDumpType(param_types[i]);
  }
  LLVMTypeRef fun_type = LLVMFunctionType(r, param_types, f.param_count, false);
  LLVMValueRef func = LLVMAddFunction(g->module, name, fun_type);
  g->current_function = func;
  g->current_function_type = fun_type;
  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(g->current_function, "entry");
  LLVMPositionBuilderAtEnd(g->builder, entry);
  for (size_t i = 0; i < f.body->as.compound.elem_count; i++) {
    ast_t *stmt = f.body->as.compound.elems[i];
    generate_statement(g, stmt);
  }
  free(types);
  free(param_types);
  free(name);
  LLVMDumpValue(func);
}

int token_to_int(token_t tok) {
  string_view_t l = tok.lexeme;
  int res = 0;
  int mult = 1;
  if (l.contents[0] == '-') {
    mult = -1;
    l.contents++;
    l.length--;
  }
  for (size_t i = 0; i < l.length; i++) {
    res = res * 10 + (l.contents[i] - '0');
  }
  return res * mult;
}

LLVMValueRef generate_expression_with_type(generator_t *g, ast_t *expr,
                                           type_t type) {
  (void)g;
  switch (expr->kind) {
  case AST_INTLIT: {
    return LLVMConstInt(type.type, token_to_int(expr->as.intlit.tok),
                        type.is_signed);
  } break;
  default: {
    printf("Unsupported expression\n");
    exit(1);
  } break;
  }
}

LLVMValueRef generate_expression(generator_t *g, ast_t *expr) {
  (void)g;
  (void)expr;
  printf("TODO: generate_expression\n");
  exit(69);
}

void generate_return(generator_t *g, ast_t *expr) {
  LLVMTypeRef current_fun_type = g->current_function_type;
  LLVMTypeRef current_ret_type = LLVMGetReturnType(current_fun_type);
  type_t ret_type = from_llvm(current_ret_type);
  LLVMValueRef expr_value = generate_expression_with_type(g, expr, ret_type);
  LLVMBuildRet(g->builder, expr_value);
}

void generate_statement(generator_t *g, ast_t *stmt) {
  switch (stmt->kind) {
  case AST_COMPOUND: {
    // TODO: save generator state and restore it afterwards, for scope purposes
    ast_compound_t c = stmt->as.compound;
    for (size_t i = 0; i < c.elem_count; i++) {
      generate_statement(g, c.elems[i]);
    }
  } break;
  case AST_RETURN: {
    generate_return(g, stmt->as.return_stmt.expr);
  }
  default: {
    // could not do that ?
  } break;
  }
}

bool is_llvm_pointer_type(LLVMTypeRef t) {
  return LLVMGetTypeKind(t) == LLVMPointerTypeKind;
}

type_t from_llvm(LLVMTypeRef t) {
  type_t res = {0};
  res.type = t;
  if (is_llvm_pointer_type(t)) {
    res.is_ptr = true;
  }
  return res;
}

type_t get_named_from_llvm(generator_t *g, LLVMTypeRef t) {
  for (size_t i = 0; i < g->nb_types; i++) {
    if (g->types[i].type == t) {
      return g->types[i];
    }
  }
  return from_llvm(t);
}