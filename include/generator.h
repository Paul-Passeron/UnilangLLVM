/**
 * generator.h
 * Copyright (C) 2024 Paul Passeron
 * GENERATOR header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include "ast.h"
#include "dynarr.h"
#include <llvm-c/Types.h>

typedef struct type_t type_t;
typedef struct class_entry_t class_entry_t;
typedef struct function_entry_t function_entry_t;
typedef struct named_value_entry_t named_value_entry_t;
typedef struct method_t method_t;

typedef enum typekind_t {
  BUILTIN,
  CLASS,
  PTR,
} typekind_t;

typedef struct types {
  type_t *items;
  size_t count;
  size_t capacity;
} types;

typedef struct classes {
  class_entry_t *items;
  size_t count;
  size_t capacity;
} classes;

typedef struct functions {
  function_entry_t *items;
  size_t count;
  size_t capacity;
} functions;

typedef struct named_values {
  named_value_entry_t *items;
  size_t count;
  size_t capacity;
} named_values;

typedef struct strings {
  char **items;
  size_t count;
  size_t capacity;
} strings;

struct type_t {
  const char *name;
  typekind_t kind;
  LLVMTypeRef type;
  union {
    type_t *pointed_by;
    class_entry_t *clazz;
  } as;
};

typedef enum specifier_t { PUBLIC, PRIVATE } specifier_t;

struct method_t {
  char *name;
  specifier_t specifier;
  strings arg_names;
  types arg_types;
  type_t return_type;
};

typedef struct member_t {
  char *name;
  specifier_t specifier;
  type_t type;
  ast_t *initializer;
} member_t;

typedef struct constructor_t {
  specifier_t specifier;
  strings arg_names;
  types arg_types;
} constructor_t;

typedef struct methods {
  method_t *items;
  size_t count;
  size_t capacity;
} methods;

typedef struct constructors {
  constructor_t *items;
  size_t count;
  size_t capacity;
} constructors;

typedef struct members {
  member_t *items;
  size_t count;
  size_t capacity;
} members;

struct class_entry_t {
  const char *name;
  methods methods;
  constructors constructors;
  members members;
};

struct function_entry_t {
  const char *name;
  strings arg_names;
  types arg_types;
  type_t return_type;
  bool is_var_args;
};

struct named_value_entry_t {
  char *name;
  type_t t;
  LLVMValueRef value;
};

typedef struct generator_t generator_t;
struct generator_t {
  LLVMContextRef context;
  LLVMModuleRef module;
  LLVMBuilderRef builder;
  functions functions;
  named_values named_values;
  types types;
  classes classes;
  function_entry_t *current_function;
};

typedef struct ltypes {
  LLVMTypeRef *items;
  size_t count;
  size_t capacity;
} ltypes;

typedef struct lvalues {
  LLVMValueRef *items;
  size_t count;
  size_t capacity;
} lvalues;

void generator_init(generator_t *generator);
void generator_free(generator_t *generator);

void set_global_generator(generator_t *g);
generator_t *get_global_generator(void);

type_t get_ptr_of(type_t t);
type_t get_type_from_name(const char *name);
type_t get_type_from_llvm(LLVMTypeRef type);

void add_type(type_t t);
void add_class(class_entry_t c);
void add_function(function_entry_t f);
void add_named_value(named_value_entry_t n);

LLVMValueRef generate_expression(ast_t *expr);
LLVMValueRef generate_cast(LLVMValueRef value, type_t target_type);
LLVMValueRef generate_intlit(token_t tok);
LLVMValueRef generate_stringlit(token_t tok);
LLVMValueRef generate_access(ast_t *access);
LLVMValueRef generate_binop(ast_t *binop);

void generate_program(ast_t *program);
void generate_decl(ast_t *decl);
void generate_fundef(ast_t *fundef);
void generate_vardef(ast_t *vardef);
void generate_classdef(ast_t *classdef);
void generate_ct_cte(ast_t *ct_cte);

void generate_statement(ast_t *stmt);
void generate_ifstmt(ast_t *if_stmt);
void generate_whilestmt(ast_t *while_stmt);
void generate_compound(ast_t *compound);
void generate_assign(ast_t *assign);
void generate_return(ast_t *ret);

LLVMValueRef fptr_from_entry(function_entry_t f);
LLVMTypeRef ftype_from_entry(function_entry_t f);
LLVMTypeRef type_to_llvm(type_t t);

type_t t_of_expr(ast_t *expr);

type_t dereference_type(type_t t);

class_entry_t entry_from_cdef(ast_class_t cdef);

int get_named_value(char *name);

LLVMValueRef get_lm_pointer(ast_t *lm);

LLVMValueRef fptr_from_constructor(constructor_t constructor,
                                   class_entry_t cdef, size_t i);
LLVMValueRef fptr_from_method(method_t method, class_entry_t cdef);

LLVMTypeRef ftype_from_constructor(constructor_t constructor,
                                   class_entry_t cdef);
LLVMTypeRef ftype_from_method(method_t method, class_entry_t cdef);

type_t get_return_type(ast_t *funcall);
#endif // GENERATOR_H
