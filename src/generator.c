/**
 * generator.c
 * Copyright (C) 2024 Paul Passeron
 * GENERATOR source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/generator.h"
#include "../include/unilang_lexer.h"
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
  add_type(g, "int", LLVMInt32TypeInContext(g->context));
  add_type(g, "char", LLVMInt8TypeInContext(g->context));

  add_type(g, "i64", LLVMInt64TypeInContext(g->context));
  add_type(g, "i32", LLVMInt32TypeInContext(g->context));
  add_type(g, "i16", LLVMInt16TypeInContext(g->context));
  add_type(g, "i8", LLVMInt8TypeInContext(g->context));

  add_unsigned_type(g, "u64", LLVMInt64TypeInContext(g->context));
  add_unsigned_type(g, "u32", LLVMInt32TypeInContext(g->context));
  add_unsigned_type(g, "u16", LLVMInt16TypeInContext(g->context));
  add_unsigned_type(g, "u8", LLVMInt8TypeInContext(g->context));

  add_type(g, "void", LLVMVoidTypeInContext(g->context));
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

  g->named_values_cap = INIT_NAMED_VALUES_CAP;
  g->named_values_vals = malloc(sizeof(LLVMValueRef) * g->named_values_cap);
  g->named_values_names = malloc(sizeof(char *) * g->named_values_cap);
  g->named_values_count = 0;

  addBuiltinTypes(g);
}

int get_named_values_scope(generator_t *g) { return g->named_values_count; }

void reset_named_values_to_scope(generator_t *g, int scope) {
  for (int i = g->named_values_count - 1; i >= scope; i--) {
    free(g->named_values_names[i]);
  }
  g->named_values_count = scope;
}

void push_named_value(generator_t *g, LLVMValueRef value, const char *name) {
  if (g->named_values_count >= g->named_values_cap) {
    LLVMValueRef *new_values =
        malloc(g->named_values_cap * 2 * sizeof(LLVMValueRef));
    char **new_names = malloc(g->named_values_cap * 2 * sizeof(char *));
    memcpy(new_values, g->named_values_vals,
           g->named_values_cap * sizeof(LLVMValueRef));
    memcpy(new_names, g->named_values_names,
           g->named_values_cap * sizeof(char *));
    free(g->named_values_vals);
    free(g->named_values_names);
    g->named_values_vals = new_values;
    g->named_values_names = new_names;
  }
  g->named_values_vals[g->named_values_count] = value;
  g->named_values_names[g->named_values_count] = strdup(name);
  g->named_values_count++;
}

void overwrite_pushed_value(generator_t *g, LLVMValueRef new_value,
                            char *name) {
  bool worked = false;
  for (int i = g->named_values_count - 1; i >= 0; --i) {
    if (strcmp(g->named_values_names[i], name) == 0) {
      g->named_values_vals[i] = new_value;
      worked = true;
      break;
    }
  }
  if (!worked) {
    push_named_value(g, new_value, name);
  }
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

int get_named_value(generator_t *g, char *name) {
  for (size_t i = 0; i < g->named_values_count; ++i) {
    if (strcmp(g->named_values_names[i], name) == 0) {
      return (int)i;
    }
  }
  return -1;
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
  }
  LLVMTypeRef fun_type = LLVMFunctionType(r, param_types, f.param_count, false);
  LLVMValueRef func = LLVMAddFunction(g->module, name, fun_type);
  g->current_function = func;
  g->current_function_type = fun_type;

  int scope = get_named_values_scope(g);

  // LLVMGetParam(func, 0)

  for (size_t i = 0; i < f.param_count; i++) {
    LLVMValueRef param = LLVMGetParam(func, i);
    char *name = strndup(f.param_names[i].lexeme.contents,
                         f.param_names[i].lexeme.length);
    push_named_value(g, param, name);
  }

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(g->current_function, "entry");
  LLVMPositionBuilderAtEnd(g->builder, entry);
  for (size_t i = 0; i < f.body->as.compound.elem_count; i++) {
    ast_t *stmt = f.body->as.compound.elems[i];
    generate_statement(g, stmt);
  }
  free(types);
  free(param_types);
  free(name);

  reset_named_values_to_scope(g, scope);

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

LLVMValueRef generate_intlit(generator_t *g, token_t tok, type_t type) {
  (void)g;
  return LLVMConstInt(type.type, token_to_int(tok), type.is_signed);
}

LLVMValueRef generate_binop(generator_t *g, token_t op, LLVMValueRef left,
                            LLVMValueRef right) {
  //
  switch (op.kind) {
  case PLUS: {
    return LLVMBuildAdd(g->builder, left, right, "");
  } break;
  default: {
    printf("Unsupported operator: " SF "\n", SA(op.lexeme));
    exit(1);
  }
  }
}

LLVMValueRef generate_expression_with_type(generator_t *g, ast_t *expr,
                                           type_t type) {
  (void)g;
  switch (expr->kind) {
  case AST_INTLIT: {
    return generate_intlit(g, expr->as.intlit.tok, type);
  } break;
  case AST_BINOP: {
    LLVMValueRef left =
        generate_expression_with_type(g, expr->as.binop.lhs, type);
    LLVMValueRef right =
        generate_expression_with_type(g, expr->as.binop.rhs, type);
    return generate_binop(g, expr->as.binop.op, left, right);
  } break;
  case AST_IDENTIFIER: {
    string_view_t sv = expr->as.identifier.tok.lexeme;
    char *name = strndup(sv.contents, sv.length);
    int named_index = get_named_value(g, name);
    if (named_index < 0) {
      printf("Identifier %s not found in the scope\n", name);
      exit(1);
    }
    free(name);
    return g->named_values_vals[named_index];
  } break;
  default: {
    printf("Unsupported expression %d\n", expr->kind);
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
    // DONE: save generator state and restore it afterwards, for scope
    // purposes
    int scope = get_named_values_scope(g);
    ast_compound_t c = stmt->as.compound;
    for (size_t i = 0; i < c.elem_count; i++) {
      generate_statement(g, c.elems[i]);
    }
    reset_named_values_to_scope(g, scope);
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