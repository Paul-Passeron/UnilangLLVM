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

typedef struct type_t {
  int is_ptr;
  int is_signed;
  const char *name;
  LLVMTypeRef type;
} type_t;

typedef struct generator_t {
  LLVMContextRef context;
  LLVMModuleRef module;
  LLVMBuilderRef builder;

  LLVMValueRef current_function;
  LLVMTypeRef current_function_type;

  type_t *types;
  size_t nb_types;
  size_t cap_types;

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

#endif // GENERATOR_H
