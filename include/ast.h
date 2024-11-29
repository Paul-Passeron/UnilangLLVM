/**
 * ast.h
 * Copyright (C) 2024 Paul Passeron
 * AST header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#ifndef AST_H
#define AST_H

#include "lexer.h"

typedef struct ast_t ast_t;

typedef enum ast_kind_t {
  AST_IDENTIFIER,
  AST_INTLIT,
  AST_FLOATLIT,
  AST_CHARLIT,
  AST_STRINGLIT,
  AST_BOOLLIT,
  AST_FUNDEF,
  AST_COMPOUND,
  AST_FUNCALL,
  AST_UNOP,
  AST_BINOP,
  AST_TYPE,
  AST_VARDEF,
  AST_CT_CTE, // compile-time constant
  AST_METHOD,
  AST_CLASS,
} ast_kind_t;

typedef struct ast_identifier_t {
  token_t tok;
} ast_identifier_t;

typedef ast_identifier_t ast_floatlit_t;
typedef ast_identifier_t ast_charlit_t;
typedef ast_identifier_t ast_stringlit_t;
typedef ast_identifier_t ast_intlit_t;

typedef struct ast_boollit_t {
  int val;
} ast_boollit_t;

typedef struct ast_fundef_t {
  token_t name;
  ast_t *return_type;
  size_t param_count;
  ast_t **param_types;
  token_t *param_names;
  ast_t *body;
} ast_fundef_t;

ast_t *new_fundef(token_t name, size_t param_count, ast_t **param_types,
                  token_t *param_names, ast_t *body, ast_t *return_type);

typedef struct tmp_param_t {
  ast_t *name;
  ast_t *type;
} tmp_param_t;

typedef struct ast_compound_t {
  size_t elem_count;
  ast_t **elems;
} ast_compound_t;

typedef ast_compound_t ast_program_t;

ast_t *new_compound(ast_t **elems);
void free_compound(ast_t *ast);

ast_t *new_program(ast_t **elems);
void free_program(ast_t *ast);

typedef struct ast_funcall_t {
  token_t called;
  size_t arg_count;
  ast_t **args;
} ast_funcall_t;

typedef struct ast_unop_t {
  token_t op;
  ast_t *operand;
} ast_unop_t;

typedef struct ast_binop_t {
  token_t op;
  ast_t *lhs;
  ast_t *rhs;
} ast_binop_t;

ast_t *new_unop(token_t op, ast_t *operand);
ast_t *new_binop(token_t op, ast_t *lhs, ast_t *rhs);

ast_t *new_funcall(token_t called, size_t arg_count, ast_t **args);

tmp_param_t *new_param(ast_t *id, ast_t *type);

ast_t *new_fundef_from_parser(token_t id, tmp_param_t **arglist,
                              ast_t *stmt_list, ast_t *return_type);

ast_t *new_identifier(token_t tok);
void free_identifier(ast_t *iden);

ast_t *new_floatlit(token_t tok);
void free_floatlit(ast_t *iden);

ast_t *new_charlit(token_t tok);
void free_charlit(ast_t *iden);

ast_t *new_stringlit(token_t tok);
void free_stringlit(ast_t *iden);

ast_t *new_intlit(token_t tok);
void free_intlit(ast_t *iden);

ast_t *new_boollit(int val);
void free_boollit(ast_t *iden);

typedef struct ast_type_t {
  token_t name;
  size_t ptr_n;
} ast_type_t;

ast_t *new_type(token_t name, size_t ptr_n);

typedef struct ast_vardef_t {
  token_t name;
  ast_t *type;
  ast_t *value; // can be NULL
} ast_vardef_t;

ast_t *new_vardef(token_t name, ast_t *type, ast_t *value);

typedef struct ast_ct_cte_t {
  token_t name;
  ast_t *value; // can be NULL
} ast_ct_cte_t;

ast_t *new_ct_cte(token_t name, ast_t *value);

typedef struct ast_method_t {
  ast_t *fdef;
  token_t specifier;
  int is_abstract;
} ast_method_t;

ast_t *new_method(ast_t *fdef, token_t specifier, int is_abstract);

typedef struct ast_class_t {
  token_t name;
  size_t field_count;
  ast_t **fields;
} ast_class_t;

ast_t *new_class(token_t name, size_t field_count, ast_t **fields);

typedef union ast_as_t {
  ast_identifier_t identifier;
  ast_floatlit_t floatlit;
  ast_charlit_t charlit;
  ast_stringlit_t stringlit;
  ast_intlit_t intlit;
  ast_boollit_t boollit;
  ast_fundef_t fundef;
  ast_compound_t compound;
  ast_program_t program;
  ast_funcall_t funcall;
  ast_unop_t unop;
  ast_binop_t binop;
  ast_type_t type;
  ast_vardef_t vardef;
  ast_ct_cte_t ct_cte;
  ast_method_t method;
  ast_class_t clazz;
} ast_as_t;

struct ast_t {
  ast_kind_t kind;
  ast_as_t as;
};

void dump_ast(ast_t *ast);
void free_ast(ast_t *ast);

#endif // AST_H
