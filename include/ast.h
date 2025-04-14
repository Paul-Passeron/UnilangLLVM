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
  AST_IFSTMT,
  AST_INDEX,
  AST_WHILE,
  AST_ASSIGN,
  AST_RETURN,
  AST_MEMBER,
  AST_AS_DIR,
  AST_NEW_DIR,
  AST_INCLUDE_DIR,
  AST_SIZE_DIR,
  AST_TEMPELEM,
  AST_INTERFACE,
  AST_TEMPLATE,
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
  // ast_t *temp;
} ast_fundef_t;

typedef struct tmp_param_t {
  ast_t *name;
  ast_t *type;
} tmp_param_t;

typedef struct ast_compound_t {
  size_t elem_count;
  ast_t **elems;
} ast_compound_t;

typedef ast_compound_t ast_program_t;

typedef struct ast_funcall_t {
  ast_t *called;
  size_t arg_count;
  ast_t **args;
  ast_t *templ;
} ast_funcall_t;

typedef struct ast_index_t {
  ast_t *subscripted;
  ast_t *index;
} ast_index_t;

typedef struct ast_unop_t {
  token_t op;
  ast_t *operand;
} ast_unop_t;

typedef struct ast_binop_t {
  token_t op;
  ast_t *lhs;
  ast_t *rhs;
} ast_binop_t;

typedef struct ast_type_t {
  token_t name;
  size_t ptr_n;
  bool is_template;
  ast_t *inst_template;
} ast_type_t;

typedef struct ast_vardef_t {
  token_t name;
  ast_t *type;
  ast_t *value; // can be NULL
} ast_vardef_t;

typedef struct ast_ct_cte_t {
  token_t name;
  ast_t *value; // can be NULL
} ast_ct_cte_t;

typedef struct ast_method_t {
  ast_t *fdef;
  token_t specifier;
  int is_abstract;
  int is_static;
} ast_method_t;

typedef struct ast_class_t {
  token_t name;
  size_t field_count;
  ast_t **fields;
  ast_t *temp;
} ast_class_t;

typedef struct ast_if_stmt_t {
  ast_t *cond;
  ast_t *body;
  ast_t *other_body;
} ast_if_stmt_t;

typedef struct ast_assign_t {
  ast_t *lhs;
  ast_t *rhs;
} ast_assign_t;

typedef struct ast_while_t {
  ast_t *cond;
  ast_t *body;
} ast_while_t;

typedef struct ast_return_t {
  ast_t *expr;
} ast_return_t;

typedef struct ast_member_t {
  ast_t *var;
  token_t specifier;
  int is_static;
} ast_member_t;

typedef struct ast_as_dir_t {
  ast_t *type;
  ast_t *expr;
} ast_as_dir_t;

typedef struct ast_include_dir_t {
  ast_t *expr;
} ast_include_dir_t;

typedef ast_as_dir_t ast_new_dir_t;

typedef struct ast_size_dir_t {
  ast_t *type;
} ast_size_dir_t;

typedef struct ast_tempelem_t {
  ast_t *type_iden;
  ast_t *interface;
} ast_tempelem_t;

typedef struct ast_interface_t {
  token_t type;
  token_t name;
  ast_t **protos;
  size_t protos_count;
} ast_interface_t;

typedef struct ast_template_t {
  ast_t **tempelems;
  size_t count;
} ast_template_t;

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
  ast_if_stmt_t if_stmt;
  ast_index_t index;
  ast_assign_t assign;
  ast_while_t while_stmt;
  ast_return_t return_stmt;
  ast_member_t member;
  ast_as_dir_t as_dir;
  ast_new_dir_t new_dir;
  ast_include_dir_t include_dir;
  ast_size_dir_t size_dir;
  ast_tempelem_t tempelem;
  ast_template_t temp;
  ast_interface_t interface;
} ast_as_t;

struct ast_t {
  ast_kind_t kind;
  ast_as_t as;
};

ast_t *new_return(ast_t *expr);

ast_t *new_while_stmt(ast_t *cond, ast_t *body);

ast_t *new_assignement(ast_t *lhs, ast_t *rhs);

ast_t *new_type(token_t name, size_t ptr_n, bool is_template, ast_t *templ);

ast_t *new_vardef(token_t name, ast_t *type, ast_t *value);

ast_t *new_ct_cte(token_t name, ast_t *value);

ast_t *new_method(ast_t *fdef, token_t specifier, int is_abstract,
                  int is_static);

ast_t *new_member(ast_t *var, token_t specifier, int is_static);

ast_t *new_class(token_t name, size_t field_count, ast_t **fields, ast_t *temp);

ast_t *new_if_stmt(ast_t *cond, ast_t *body, ast_t *other_body);

ast_t *new_fundef(token_t name, size_t param_count, ast_t **param_types,
                  token_t *param_names, ast_t *body, ast_t *return_type);

ast_t *new_compound(ast_t **elems);
void free_compound(ast_t *ast);

ast_t *new_program(ast_t **elems);
void free_program(ast_t *ast);

ast_t *new_unop(token_t op, ast_t *operand);

ast_t *new_binop(token_t op, ast_t *lhs, ast_t *rhs);

ast_t *new_funcall(ast_t *called, size_t arg_count, ast_t **args);

ast_t *new_index(ast_t *subscripted, ast_t *index);

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

ast_t *new_as_dir(ast_t *type, ast_t *expr);
void free_as_dir(ast_t *ast);

ast_t *new_new_dir(ast_t *type, ast_t *expr);
void free_new_dir(ast_t *ast);

ast_t *new_include_dir(ast_t *expr);
void free_include_dir(ast_t *ast);

ast_t *new_size_dir(ast_t *type);
void free_size_dir(ast_t *ast);

ast_t *new_tempelem(ast_t *t, ast_t *interface);
void free_tempelem(ast_t *ast);

ast_t *new_interface(token_t type, token_t name, ast_t **protos,
                     size_t protos_count);
void free_interface(ast_t *ast);

ast_t *new_template(ast_t **elems);
void free_template(ast_t *ast);

void dump_ast(ast_t *ast);
void free_ast(ast_t *ast);

#endif // AST_H
