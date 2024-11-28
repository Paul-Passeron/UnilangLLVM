#include "../include/unilang_parser.h"
token_t *parse_token_lexeme(lexer_t *l, int *worked, string_view_t lexeme) {
  token_t tok = next(l);
  *worked = 0;
  if (is_error_tok(tok)) {
    return NULL;
  }
  if (sv_eq(tok.lexeme, lexeme)) {
    token_t *res = malloc(sizeof(token_t));
    *res = tok;
    *worked = 1;
    return res;
  }
  return NULL;
}

token_t *parse_token_kind(lexer_t *l, int *worked, int kind) {
  token_t tok = next(l);
  *worked = 0;
  if (is_error_tok(tok)) {
    return NULL;
  }
  if (tok.kind == kind) {
    token_t *res = malloc(sizeof(token_t));
    *res = tok;
    *worked = 1;
    return res;
  }
  return NULL;
}

// RULE identifier
void *parse_identifier_c0(lexer_t *l, int *worked);

// RULE intlit
void *parse_intlit_c0(lexer_t *l, int *worked);

// RULE floatlit
void *parse_floatlit_c0(lexer_t *l, int *worked);

// RULE charlit
void *parse_charlit_c0(lexer_t *l, int *worked);

// RULE stringlit
void *parse_stringlit_c0(lexer_t *l, int *worked);

// RULE boollit
void *parse_boollit_c0(lexer_t *l, int *worked);

void *parse_boollit_c1(lexer_t *l, int *worked);

// RULE literal
void *parse_literal_c0(lexer_t *l, int *worked);

void *parse_literal_c1(lexer_t *l, int *worked);

void *parse_literal_c2(lexer_t *l, int *worked);

void *parse_literal_c3(lexer_t *l, int *worked);

// RULE param
void *parse_param_c0(lexer_t *l, int *worked);

// RULE arglist
// RULE funcall
void *parse_funcall_c0(lexer_t *l, int *worked);

// RULE funcallargs
// RULE leaf
void *parse_leaf_c0(lexer_t *l, int *worked);

void *parse_leaf_c1(lexer_t *l, int *worked);

void *parse_leaf_c2(lexer_t *l, int *worked);

void *parse_leaf_c3(lexer_t *l, int *worked);

// RULE expr
void *parse_expr_c0(lexer_t *l, int *worked);

void *parse_expr_c1(lexer_t *l, int *worked);

// RULE stmt
void *parse_stmt_c0(lexer_t *l, int *worked);

// RULE decl
void *parse_decl_c0(lexer_t *l, int *worked);

// RULE binop
void *parse_binop_c0(lexer_t *l, int *worked);

// RULE paren
void *parse_paren_c0(lexer_t *l, int *worked);

// RULE unary
void *parse_unary_c0(lexer_t *l, int *worked);

// RULE stmt_list
// RULE program_list
// RULE program
void *parse_program_c0(lexer_t *l, int *worked);

// RULE fundef
void *parse_fundef_c0(lexer_t *l, int *worked);

// RULE uop
void *parse_uop_c0(lexer_t *l, int *worked);

void *parse_uop_c1(lexer_t *l, int *worked);

// RULE identifier
void *parse_identifier(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_identifier_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE intlit
void *parse_intlit(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_intlit_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE floatlit
void *parse_floatlit(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_floatlit_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE charlit
void *parse_charlit(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_charlit_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE stringlit
void *parse_stringlit(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_stringlit_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE boollit
void *parse_boollit(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_boollit_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_boollit_c1(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE literal
void *parse_literal(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_literal_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_literal_c1(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_literal_c2(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_literal_c3(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE param
void *parse_param(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_param_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

void *parse_arglist(lexer_t *l, int *worked) {
  int rule_worked = 0;
  size_t cap = 16;
  size_t count = 0;
  void **elems = malloc(sizeof(void *) * cap);
  while (1) {
    int worked = 0;
    void *elem = parse_param(l, &rule_worked);
    lexer_t old = *l;
    if (!rule_worked) {
      *l = old;
      break;
    }
    if (count >= cap) {
      cap *= 2;
      elem = realloc(elem, cap * sizeof(void *));
    }
    elems[count++] = elem;
    void *sep = parse_token_lexeme(l, &rule_worked, SV(","));
    if (!rule_worked) {
      *l = old;
      break;
    }
  }
  elems = realloc(elems, (count + 1) * sizeof(void *));
  elems[count] = NULL;
  *worked = count > 0;
  return elems;
}
// RULE funcall
void *parse_funcall(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_funcall_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

void *parse_funcallargs(lexer_t *l, int *worked) {
  int rule_worked = 0;
  size_t cap = 16;
  size_t count = 0;
  void **elems = malloc(sizeof(void *) * cap);
  while (1) {
    int worked = 0;
    void *elem = parse_expr(l, &rule_worked);
    lexer_t old = *l;
    if (!rule_worked) {
      *l = old;
      break;
    }
    if (count >= cap) {
      cap *= 2;
      elem = realloc(elem, cap * sizeof(void *));
    }
    elems[count++] = elem;
    void *sep = parse_token_lexeme(l, &rule_worked, SV(","));
    if (!rule_worked) {
      *l = old;
      break;
    }
  }
  elems = realloc(elems, (count + 1) * sizeof(void *));
  elems[count] = NULL;
  *worked = count > 0;
  return elems;
}
// RULE leaf
void *parse_leaf(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_leaf_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_leaf_c1(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_leaf_c2(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_leaf_c3(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE expr
void *parse_expr(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_expr_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_expr_c1(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE stmt
void *parse_stmt(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_stmt_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE decl
void *parse_decl(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_decl_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE binop
void *parse_binop(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_binop_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE paren
void *parse_paren(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_paren_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE unary
void *parse_unary(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_unary_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

void *parse_stmt_list(lexer_t *l, int *worked) {
  int rule_worked = 0;
  size_t cap = 16;
  size_t count = 0;
  void **elems = malloc(sizeof(void *) * cap);
  while (1) {
    int worked = 0;
    void *elem = parse_stmt(l, &rule_worked);
    lexer_t old = *l;
    if (!rule_worked) {
      *l = old;
      break;
    }
    if (count >= cap) {
      cap *= 2;
      elem = realloc(elem, cap * sizeof(void *));
    }
    elems[count++] = elem;
  }
  elems = realloc(elems, (count + 1) * sizeof(void *));
  elems[count] = NULL;
  *worked = count > 0;
  return elems;
}
void *parse_program_list(lexer_t *l, int *worked) {
  int rule_worked = 0;
  size_t cap = 16;
  size_t count = 0;
  void **elems = malloc(sizeof(void *) * cap);
  while (1) {
    int worked = 0;
    void *elem = parse_decl(l, &rule_worked);
    lexer_t old = *l;
    if (!rule_worked) {
      *l = old;
      break;
    }
    if (count >= cap) {
      cap *= 2;
      elem = realloc(elem, cap * sizeof(void *));
    }
    elems[count++] = elem;
  }
  elems = realloc(elems, (count + 1) * sizeof(void *));
  elems[count] = NULL;
  *worked = count > 0;
  return elems;
}
// RULE program
void *parse_program(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_program_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE fundef
void *parse_fundef(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_fundef_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE uop
void *parse_uop(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_uop_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_uop_c1(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

void *parse_identifier_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_token_kind(l, worked, IDENTIFIER);
  if (!*worked) {
    return NULL;
  }

  token_t *ptr = elem_0;
  ast_t *res = new_identifier(*ptr);
  free(ptr);
  return res;
}
void *parse_intlit_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_token_kind(l, worked, INTLIT);
  if (!*worked) {
    return NULL;
  }

  token_t *ptr = elem_0;
  ast_t *res = new_intlit(*ptr);
  free(ptr);
  return res;
}
void *parse_floatlit_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_token_kind(l, worked, FLOATLIT);
  if (!*worked) {
    return NULL;
  }

  token_t *ptr = elem_0;
  ast_t *res = new_floatlit(*ptr);
  free(ptr);
  return res;
}
void *parse_charlit_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_token_kind(l, worked, CHARLIT);
  if (!*worked) {
    return NULL;
  }

  token_t *ptr = elem_0;
  ast_t *res = new_charlit(*ptr);
  free(ptr);
  return res;
}
void *parse_stringlit_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_token_kind(l, worked, STRLIT);
  if (!*worked) {
    return NULL;
  }

  token_t *ptr = elem_0;
  ast_t *res = new_stringlit(*ptr);
  free(ptr);
  return res;
}
void *parse_boollit_c0(lexer_t *l, int *worked) {
  *worked = 0;
  free(parse_token_lexeme(l, worked, SV("true")));
  if (!*worked) {
    return NULL;
  }
  return new_boollit(1);
}
void *parse_boollit_c1(lexer_t *l, int *worked) {
  *worked = 0;
  free(parse_token_lexeme(l, worked, SV("false")));
  if (!*worked) {
    return NULL;
  }
  return new_boollit(0);
}
void *parse_literal_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_intlit(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_literal_c1(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_floatlit(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_literal_c2(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_charlit(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_literal_c3(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_stringlit(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_param_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_identifier(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV(":")));
  if (!*worked) {
    return NULL;
  }
  void *elem_2 = parse_identifier(l, worked);
  if (!*worked) {
    return NULL;
  }

  ast_t *id = elem_0;
  ast_t *type = elem_2;
  return new_param(id, type);
}
void *parse_funcall_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_identifier(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("(")));
  if (!*worked) {
    return NULL;
  }
  void *elem_2 = parse_funcallargs(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV(")")));
  if (!*worked) {
    return NULL;
  }

  ast_t *iden = elem_0;
  ast_t **elems = elem_2;
  token_t name = iden->as.identifier.tok;
  free_ast(iden);
  size_t i = 0;
  while (elems[i]) {
    i++;
  }
  elems = realloc(elems, sizeof(ast_t *) * i);
  return new_funcall(name, i, elems);
}
void *parse_leaf_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_paren(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_leaf_c1(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_funcall(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_leaf_c2(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_literal(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_leaf_c3(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_identifier(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_expr_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_binop(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_expr_c1(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_leaf(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_stmt_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_expr(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV(";")));
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_decl_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_fundef(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_binop_c0(lexer_t *l, int *worked) {
  *worked = 0;

  *worked = 1;
  ast_t *res = parse_expression_aux(l, -1);
  if (!res)
    *worked = 0;
  return res;
}
void *parse_paren_c0(lexer_t *l, int *worked) {
  *worked = 0;
  free(parse_token_lexeme(l, worked, SV("(")));
  if (!*worked) {
    return NULL;
  }
  void *elem_1 = parse_expr(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV(")")));
  if (!*worked) {
    return NULL;
  }

  return elem_1;
}
void *parse_unary_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_uop(l, worked);
  if (!*worked) {
    return NULL;
  }
  void *elem_1 = parse_leaf(l, worked);
  if (!*worked) {
    return NULL;
  }

  token_t *ptr = elem_0;
  ast_t *leaf = elem_1;
  ast_t *res = new_unop(*ptr, leaf);
  return res;
}
void *parse_program_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_program_list(l, worked);
  if (!*worked) {
    return NULL;
  }

  return new_compound(elem_0);
}
void *parse_fundef_c0(lexer_t *l, int *worked) {
  *worked = 0;
  free(parse_token_lexeme(l, worked, SV("let")));
  if (!*worked) {
    return NULL;
  }
  void *elem_1 = parse_identifier(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("(")));
  if (!*worked) {
    return NULL;
  }
  void *elem_3 = parse_arglist(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV(")")));
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("=>")));
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("{")));
  if (!*worked) {
    return NULL;
  }
  void *elem_7 = parse_stmt_list(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("}")));
  if (!*worked) {
    return NULL;
  }

  ast_t *id = elem_1;
  tmp_param_t **tmp_arglist = elem_3;
  ast_t **stmt_list = elem_7;
  token_t tok = id->as.identifier.tok;
  free_ast(id);
  return new_fundef_from_parser(tok, tmp_arglist, stmt_list);
}
void *parse_uop_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_token_lexeme(l, worked, SV("-"));
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_uop_c1(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_token_lexeme(l, worked, SV("not"));
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
