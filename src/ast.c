/**
 * ast.c
 * Copyright (C) 2024 Paul Passeron
 * AST source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void dump_type(ast_t *t) {
  ast_type_t type = t->as.type;
  fflush(stdout);
  printf("\"" SF, SA(type.name.lexeme));
  for (size_t i = 0; i < type.ptr_n; i++) {
    printf("*");
  }
  printf("\"");
}

void dump_ast(ast_t *ast) {
  if (ast == NULL) {
    printf("null");
    return;
  }
  if (ast->kind == AST_TYPE) {
    dump_type(ast);
    return;
  }
  printf("{");
  printf("\"kind\": ");
  switch (ast->kind) {
  case AST_IDENTIFIER:
    printf("\"AST_IDENTIFIER\",");
    printf("\"value\": \"");
    print_token(ast->as.identifier.tok);
    printf("\"");
    break;
  case AST_INTLIT:
    printf("\"AST_INTLIT\",");
    printf("\"value\": \"" SF "\"",
           SA(ast->as.intlit.tok.lexeme)); // Assuming tok.value is a string
                                           // representation of the integer
    break;
  case AST_FLOATLIT:
    printf("\"AST_FLOATLIT\",");
    printf("\"value\": \"" SF "\"", SA(ast->as.floatlit.tok.lexeme));
    break;
  case AST_CHARLIT:
    printf("\"AST_CHARLIT\",");
    printf("\"value\": \"" SF "\"", SA(ast->as.charlit.tok.lexeme));
    break;
  case AST_STRINGLIT:
    printf("\"AST_STRINGLIT\",");
    printf("\"value\": " SF, SA(ast->as.stringlit.tok.lexeme));
    break;
  case AST_BOOLLIT:
    printf("\"AST_BOOLLIT\",");
    printf("\"value\": %s", ast->as.boollit.val ? "true" : "false");
    break;
  case AST_FUNDEF: {
    printf("\"AST_FUNDEF\",");
    printf("\"name\": \"");
    print_token(ast->as.fundef.name);
    printf("\", ");
    if (ast->as.fundef.return_type) {
      printf("\"return_type\": ");
      dump_type(ast->as.fundef.return_type);
      printf(", ");
    }
    printf("\"params\": [");
    for (size_t i = 0; i < ast->as.fundef.param_count; ++i) {
      if (i > 0) {
        printf(", ");
      }
      printf("{\"name\": \"");
      print_token(ast->as.fundef.param_names[i]);
      printf("\", \"type\":");
      dump_ast(ast->as.fundef.param_types[i]);
      printf("}");
    }
    printf("],");
    printf("\"body\": ");
    dump_ast(ast->as.fundef.body);
  } break;
  case AST_COMPOUND: {
    printf("\"AST_COMPOUND\",");
    printf("\"content\": [");
    for (size_t i = 0; i < ast->as.compound.elem_count; ++i) {
      if (i > 0)
        printf(",");
      dump_ast(ast->as.compound.elems[i]);
    }
    printf("]");
  } break;
  case AST_FUNCALL: {
    printf("\"AST_FUNCALL\",");
    printf("\"called\":");
    dump_ast(ast->as.funcall.called);
    printf(", \"args\": [");
    for (size_t i = 0; i < ast->as.funcall.arg_count; i++) {
      if (i > 0) {
        printf(",");
      }
      dump_ast(ast->as.funcall.args[i]);
    }
    printf("]");
  } break;
  case AST_BINOP: {
    printf("\"AST_BINOP\",");
    printf("\"operator\": \"");
    print_token(ast->as.binop.op);
    printf("\",\"lhs\":");
    dump_ast(ast->as.binop.lhs);
    printf(",\"rhs\":");
    dump_ast(ast->as.binop.rhs);
  } break;
  case AST_VARDEF: {
    printf("\"AST_VARDEF\",");
    printf("\"name\": \"");
    print_token(ast->as.vardef.name);
    printf("\", \"type\":");
    dump_type(ast->as.vardef.type);
    printf(",\"value\":");
    dump_ast(ast->as.vardef.value);
  } break;
  case AST_CT_CTE: {
    printf("\"AST_CT_CTE\",");
    printf("\"name\": \"");
    print_token(ast->as.ct_cte.name);
    printf("\",\"value\":");
    dump_ast(ast->as.ct_cte.value);
  } break;
  case AST_METHOD: {
    printf("\"AST_METHOD\", ");
    printf("\"abstract\": \"%s\", ",
           ast->as.method.is_abstract ? "true" : "false");
    printf("\"static\": \"%s\", ", ast->as.method.is_static ? "true" : "false");
    printf("\"specifier\": \"");
    print_token(ast->as.method.specifier);
    printf("\", ");
    printf("\"fun\": ");
    dump_ast(ast->as.method.fdef);
  } break;
  case AST_MEMBER: {
    printf("\"AST_MEMBER\", ");
    printf("\"static\": \"%s\", ", ast->as.member.is_static ? "true" : "false");
    printf("\"specifier\": \"");
    print_token(ast->as.method.specifier);
    printf("\", ");
    printf("\"var\": ");
    dump_ast(ast->as.member.var);
  } break;
  case AST_CLASS: {
    printf("\"AST_CLASS\", ");
    printf("\"name\": \"");
    print_token(ast->as.clazz.name);
    printf("\", \"fields\": [");
    for (size_t i = 0; i < ast->as.clazz.field_count; ++i) {
      if (i > 0)
        printf(", ");
      dump_ast(ast->as.clazz.fields[i]);
    }
    printf("]");
  } break;
  case AST_IFSTMT: {
    printf("\"AST_IFSTMT\", ");
    printf("\"cond\": ");
    dump_ast(ast->as.if_stmt.cond);
    printf(",\"if\": ");
    dump_ast(ast->as.if_stmt.body);
    printf(", \"else\": ");
    dump_ast(ast->as.if_stmt.other_body);
  } break;
  case AST_INDEX: {
    printf("\"AST_INDEX\",");
    printf("\"subscripted\":");
    dump_ast(ast->as.index.subscripted);
    printf(", \"index\": ");
    dump_ast(ast->as.index.index);
  } break;
  case AST_ASSIGN: {
    printf("\"AST_ASSIGN\",");
    printf("\"lhs\": ");
    dump_ast(ast->as.assign.lhs);
    printf(", \"rhs\": ");
    dump_ast(ast->as.assign.rhs);
  } break;

  case AST_WHILE: {
    printf("\"AST_WHILE\",");
    printf("\"cond\": ");
    dump_ast(ast->as.while_stmt.cond);
    printf(", \"body\": ");
    dump_ast(ast->as.while_stmt.body);
  } break;
  case AST_RETURN: {
    printf("\"AST_RETURN\",");
    printf("\"expr\": ");
    dump_ast(ast->as.return_stmt.expr);
  } break;
  case AST_UNOP: {
    printf("\"AST_UNOP\",");
    printf("\"op\": ");
    printf("\"" SF "\"", SA(ast->as.unop.op.lexeme));
    printf(", \"operand\": ");
    dump_ast(ast->as.unop.operand);
  } break;
  case AST_AS_DIR: {
    printf("\"AST_AS_DIR\",");
    printf("\"type\": ");
    dump_ast(ast->as.as_dir.type);
    printf(", \"expr\": ");
    dump_ast(ast->as.as_dir.expr);
  } break;
  case AST_INCLUDE_DIR: {
    printf("\"AST_INCLUDE_DIR\",\"included\": ");
    dump_ast(ast->as.include_dir.expr);
  } break;
  case AST_SIZE_DIR: {
    printf("\"AST_SIZE_DIR\", \"type\": ");
    dump_ast(ast->as.size_dir.type);
  } break;
  case AST_TEMPELEM: {
    printf("\"AST_TEMPELEM\"");
    // TODO !
  } break;
  case AST_INTERFACE: {
    printf("\"AST_INTERFACE\", ");
    printf("\"name\": \"" SF "\",", SA(ast->as.interface.name.lexeme));
    printf("\"type\": \"" SF "\",", SA(ast->as.interface.type.lexeme));
    printf("\"protos\": [");
    for (size_t i = 0; i < ast->as.interface.protos_count; ++i) {
      if (i > 0) {
        printf(", ");
      }
      dump_ast(ast->as.interface.protos[i]);
    }
    printf("]");
  } break;
  default:
    printf("\"UNKNOWN_KIND\"");
    break;
  }
  printf("}");
}

void free_fundef(ast_t *ast) {
  (void)ast;
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
    printf("TODO AST_COMPOUND\n");
    exit(1);
    break;
  case AST_FUNCALL: {
    printf("TODO AST FUNCALL\n");
    exit(1);
  } break;
  case AST_UNOP: {
    printf("TODO AST_UNOP\n");
    exit(1);
  } break;
  case AST_BINOP: {
    printf("TODO AST_BINOP\n");
    exit(1);
  } break;
  case AST_TYPE: {
    printf("TODO AST_TYPE\n");
    exit(1);
  } break;
  case AST_VARDEF: {
    printf("TODO AST_VARDEF\n");
    exit(1);
  } break;
  case AST_CT_CTE: {
    printf("TODO AST_CT_CTE\n");
    exit(1);
  } break;
  case AST_METHOD: {
    printf("TODO AST_METHOD\n");
    exit(1);
  } break;
  case AST_CLASS: {
    printf("TODO AST_CLASS\n");
    exit(1);
  } break;
  case AST_IFSTMT: {
    printf("TODO: AST_IFSTMT\n");
    exit(1);
  } break;
  case AST_INDEX: {
    printf("TODO AST_INDEX\n");
    exit(1);
  } break;
  case AST_WHILE: {
    printf("TODO AST_WHILE\n");
    exit(1);
  } break;
  case AST_ASSIGN: {
    printf("TODO AST_ASSIGN\n");
    exit(1);
  } break;
  case AST_RETURN: {
    printf("TODO AST_RETURN\n");
    exit(1);
  } break;
  case AST_MEMBER: {
    printf("TODO AST_MEMBER\n");
    exit(1);
  } break;
  case AST_AS_DIR: {
    printf("TODO AST_AS_DIR\n");
    exit(1);
  }
  case AST_NEW_DIR: {
    printf("TODO AST_NEW_DIR\n");
    exit(1);
  }
  case AST_INCLUDE_DIR: {
    printf("TODO: AST_INCLUDE_DIR\n");
    exit(1);
  }
  case AST_SIZE_DIR: {
    printf("TODO: AST_SIZE_DIR\n");
    exit(1);
  }
  case AST_INTERFACE: {
    printf("TODO: AST_INTERFACE\n");
    exit(1);
  }
  case AST_TEMPELEM: {
    printf("TODO: AST_TEMPELEM\n");
    exit(1);
  }
  }
}

ast_t *new_fundef_from_parser(token_t id, tmp_param_t **arglist,
                              ast_t *stmt_list, ast_t *return_type) {
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
  return new_fundef(id, i, param_types, param_names, stmt_list, return_type);
}

tmp_param_t *new_param(ast_t *id, ast_t *type) {
  tmp_param_t *res = malloc(sizeof(tmp_param_t));
  res->type = type;
  res->name = id;
  return res;
}

ast_t *new_fundef(token_t name, size_t param_count, ast_t **param_types,
                  token_t *param_names, ast_t *body, ast_t *return_type) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_FUNDEF;
  res->as.fundef.name = name;
  res->as.fundef.param_count = param_count;
  res->as.fundef.param_types = param_types;
  res->as.fundef.param_names = param_names;
  res->as.fundef.body = body;
  res->as.fundef.return_type = return_type;
  return res;
}

ast_t *new_compound(ast_t **elems) {
  size_t count = 0;
  while (elems[count]) {
    count++;
  }
  ast_t *res = malloc(sizeof(ast_t));
  ast_t **new_elems = malloc(sizeof(ast_t *) * count);
  memcpy(new_elems, elems, sizeof(ast_t *) * count);
  free(elems);
  res->kind = AST_COMPOUND;
  res->as.compound = (ast_compound_t){count, new_elems};
  return res;
}

void free_compound(ast_t *ast) {
  (void)ast;
  // TODO
}

ast_t *new_funcall(ast_t *called, size_t arg_count, ast_t **args) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_FUNCALL;
  // TODO: handle templated types
  res->as.funcall = (ast_funcall_t){called, arg_count, args, NULL};
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

ast_t *new_type(token_t name, size_t ptr_n) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_TYPE;
  res->as.type = (ast_type_t){name, ptr_n};
  return res;
}

ast_t *new_vardef(token_t name, ast_t *type, ast_t *value) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_VARDEF;
  res->as.vardef = (ast_vardef_t){name, type, value};
  return res;
}

ast_t *new_ct_cte(token_t name, ast_t *value) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_CT_CTE;
  res->as.ct_cte = (ast_ct_cte_t){name, value};
  return res;
}

ast_t *new_method(ast_t *fdef, token_t specifier, int is_abstract,
                  int is_static) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_METHOD;
  res->as.method = (ast_method_t){
      fdef,
      specifier,
      is_abstract,
      is_static,
  };
  return res;
}

ast_t *new_member(ast_t *fdef, token_t specifier, int is_static) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_MEMBER;
  res->as.member = (ast_member_t){
      fdef,
      specifier,
      is_static,
  };
  return res;
}

ast_t *new_class(token_t name, size_t field_count, ast_t **fields,
                 ast_t *temp) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_CLASS;
  res->as.clazz = (ast_class_t){name, field_count, fields, temp};
  return res;
}

ast_t *new_if_stmt(ast_t *cond, ast_t *body, ast_t *other_body) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_IFSTMT;
  res->as.if_stmt = (ast_if_stmt_t){cond, body, other_body};
  return res;
}

ast_t *new_index(ast_t *subscripted, ast_t *index) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_INDEX;
  res->as.index = (ast_index_t){subscripted, index};
  return res;
}

ast_t *new_while_stmt(ast_t *cond, ast_t *body) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_WHILE;
  res->as.while_stmt = (ast_while_t){cond, body};
  return res;
}

ast_t *new_assignement(ast_t *lhs, ast_t *rhs) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_ASSIGN;
  res->as.assign = (ast_assign_t){lhs, rhs};
  return res;
}

ast_t *new_return(ast_t *expr) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_RETURN;
  res->as.return_stmt = (ast_return_t){expr};
  return res;
}

ast_t *new_as_dir(ast_t *type, ast_t *expr) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_AS_DIR;
  res->as.as_dir = (ast_as_dir_t){type, expr};
  return res;
}

ast_t *new_new_dir(ast_t *type, ast_t *expr) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_NEW_DIR;
  res->as.as_dir = (ast_new_dir_t){type, expr};
  return res;
}

ast_t *new_include_dir(ast_t *expr) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_INCLUDE_DIR;
  res->as.include_dir.expr = expr;
  return res;
}

ast_t *new_size_dir(ast_t *type) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_SIZE_DIR;
  res->as.size_dir.type = type;
  return res;
}

ast_t *new_tempelem(ast_t *t, ast_t *interface) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_TEMPELEM;
  res->as.tempelem = (ast_tempelem_t){t, interface};
  return res;
}

ast_t *new_interface(token_t type, token_t name, ast_t **protos,
                     size_t protos_count) {
  ast_t *res = malloc(sizeof(ast_t));
  res->kind = AST_INTERFACE;
  res->as.interface = (ast_interface_t){type, name, protos, protos_count};
  return res;
}