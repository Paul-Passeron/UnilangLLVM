/**
 * generator.h
 * Copyright (C) 2024 Paul Passeron
 * GENERATOR header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

// File created by the new_file tool !

#ifndef GENERATOR_H
#define GENERATOR_H

#include "ast.h"
#include <llvm-c/Types.h>

#define INIT_TYPES_CAP 16
#define INIT_NAMED_VALUES_CAP 16
#define INIT_FUNCS_CAP 16
#define INIT_CLASSES_CAP 16
typedef struct type_t type_t;
struct type_t {
  int is_ptr;
  int is_signed;
  int is_class;
  const char *name;
  LLVMTypeRef type;
  ast_t *classdecl;
  type_t *pointed_to;
};

typedef struct generator_t {
  LLVMContextRef context;
  LLVMModuleRef module;
  LLVMBuilderRef builder;

  LLVMValueRef current_function;
  LLVMTypeRef current_function_type;

  LLVMValueRef *named_values_vals;
  type_t *named_values_types;
  char **named_values_names;
  size_t named_values_count;
  size_t named_values_cap;

  type_t *types;
  size_t nb_types;
  size_t cap_types;

  ast_t **funcs;
  size_t funcs_count;
  size_t funcs_cap;

  // ast_t **classes;
  // size_t classes_count;
  // size_t classes_cap;

} generator_t;

void init(generator_t *g, const char *module_name);

void add_type(generator_t *g, const char *name, LLVMTypeRef type);

void add_unsigned_type(generator_t *g, const char *name, LLVMTypeRef type);

type_t get_type_from_name(generator_t *g, const char *name);

type_t get_ptr_of(type_t t);

int is_type_defined(generator_t *g, const char *name);

void generate_function(generator_t *g, ast_t *fun);

void generate_program(generator_t *g, ast_t *program);

LLVMValueRef generate_expression(generator_t *g, ast_t *expr);

LLVMValueRef generate_expression_with_type(generator_t *g, ast_t *expr,
                                           type_t type);

void generate_statement(generator_t *g, ast_t *stmt);

type_t from_llvm(LLVMTypeRef t);

bool is_llvm_pointer_type(LLVMTypeRef t);

type_t get_named_from_llvm(generator_t *g, LLVMTypeRef t);

ast_t *get_function_ast_from_name(generator_t *g, char *fname);

void add_function_ast(generator_t *g, ast_t *fun);

bool is_integer_type(generator_t *g, type_t t);

void push_named_value(generator_t *g, LLVMValueRef value, const char *name,
                      type_t t);

LLVMTypeRef generate_funtype(generator_t *g, ast_t *fun);

type_t get_type_from_expr(generator_t *g, ast_t *expr);

#endif // GENERATOR_H
