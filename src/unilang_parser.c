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
// RULE funcallargs
// RULE leaf
void *parse_leaf_c0(lexer_t *l, int *worked);

void *parse_leaf_c1(lexer_t *l, int *worked);

void *parse_leaf_c2(lexer_t *l, int *worked);

// RULE expr
void *parse_expr_c0(lexer_t *l, int *worked);

void *parse_expr_c1(lexer_t *l, int *worked);

// RULE stmt
void *parse_stmt_c0(lexer_t *l, int *worked);

void *parse_stmt_c1(lexer_t *l, int *worked);

void *parse_stmt_c2(lexer_t *l, int *worked);

void *parse_stmt_c3(lexer_t *l, int *worked);

void *parse_stmt_c4(lexer_t *l, int *worked);

void *parse_stmt_c5(lexer_t *l, int *worked);

void *parse_stmt_c6(lexer_t *l, int *worked);

// RULE decl
void *parse_decl_c0(lexer_t *l, int *worked);

void *parse_decl_c1(lexer_t *l, int *worked);

void *parse_decl_c2(lexer_t *l, int *worked);

void *parse_decl_c3(lexer_t *l, int *worked);

// RULE binop
void *parse_binop_c0(lexer_t *l, int *worked);

// RULE paren
void *parse_paren_c0(lexer_t *l, int *worked);

// RULE starlist
// RULE type
void *parse_type_c0(lexer_t *l, int *worked);

void *parse_type_c1(lexer_t *l, int *worked);

// RULE unary
void *parse_unary_c0(lexer_t *l, int *worked);

// RULE stmt_list
// RULE compound
void *parse_compound_c0(lexer_t *l, int *worked);

void *parse_compound_c1(lexer_t *l, int *worked);

// RULE program_list
// RULE program
void *parse_program_c0(lexer_t *l, int *worked);

// RULE fundef_letless
void *parse_fundef_letless_c0(lexer_t *l, int *worked);

void *parse_fundef_letless_c1(lexer_t *l, int *worked);

// RULE fundef
void *parse_fundef_c0(lexer_t *l, int *worked);

// RULE uop
void *parse_uop_c0(lexer_t *l, int *worked);

void *parse_uop_c1(lexer_t *l, int *worked);

// RULE vardef_letless
void *parse_vardef_letless_c0(lexer_t *l, int *worked);

void *parse_vardef_letless_c1(lexer_t *l, int *worked);

// RULE vardef
void *parse_vardef_c0(lexer_t *l, int *worked);

// RULE ct_cte
void *parse_ct_cte_c0(lexer_t *l, int *worked);

// RULE access_spec
void *parse_access_spec_c0(lexer_t *l, int *worked);

void *parse_access_spec_c1(lexer_t *l, int *worked);

// RULE abstract_opt
void *parse_abstract_opt_c0(lexer_t *l, int *worked);

void *parse_abstract_opt_c1(lexer_t *l, int *worked);

// RULE static_opt
void *parse_static_opt_c0(lexer_t *l, int *worked);

void *parse_static_opt_c1(lexer_t *l, int *worked);

// RULE class_body
// RULE class_body_item
void *parse_class_body_item_c0(lexer_t *l, int *worked);

void *parse_class_body_item_c1(lexer_t *l, int *worked);

// RULE class_decl
void *parse_class_decl_c0(lexer_t *l, int *worked);

// RULE if_statement
void *parse_if_statement_c0(lexer_t *l, int *worked);

void *parse_if_statement_c1(lexer_t *l, int *worked);

// RULE while_stmt
void *parse_while_stmt_c0(lexer_t *l, int *worked);

// RULE assignement
void *parse_assignement_c0(lexer_t *l, int *worked);

// RULE return
void *parse_return_c0(lexer_t *l, int *worked);

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
    lexer_t old = *l;
    void *elem = parse_param(l, &rule_worked);
    if (!rule_worked) {
      break;
    }
    if (count >= cap) {
      cap *= 2;
      elem = realloc(elem, cap * sizeof(void *));
    }
    elems[count++] = elem;
    old = *l;
    (void)parse_token_lexeme(l, &rule_worked, SV(","));
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
void *parse_funcallargs(lexer_t *l, int *worked) {
  int rule_worked = 0;
  size_t cap = 16;
  size_t count = 0;
  void **elems = malloc(sizeof(void *) * cap);
  while (1) {
    lexer_t old = *l;
    void *elem = parse_expr(l, &rule_worked);
    if (!rule_worked) {
      break;
    }
    if (count >= cap) {
      cap *= 2;
      elem = realloc(elem, cap * sizeof(void *));
    }
    elems[count++] = elem;
    old = *l;
    (void)parse_token_lexeme(l, &rule_worked, SV(","));
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
  rule_cpy = *l;
  rule_res = parse_stmt_c1(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_stmt_c2(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_stmt_c3(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_stmt_c4(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_stmt_c5(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_stmt_c6(&rule_cpy, &rule_worked);
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
  rule_cpy = *l;
  rule_res = parse_decl_c1(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_decl_c2(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_decl_c3(&rule_cpy, &rule_worked);
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

void *parse_starlist(lexer_t *l, int *worked) {
  int rule_worked = 0;
  size_t cap = 16;
  size_t count = 0;
  void **elems = malloc(sizeof(void *) * cap);
  while (1) {
    lexer_t old = *l;
    void *elem = parse_token_lexeme(l, &rule_worked, SV("*"));
    if (!rule_worked) {
      *l = old;
      break;
    }
    if (count >= cap) {
      cap *= 2;
      elem = realloc(elem, cap * sizeof(void *));
    }
    elems[count++] = elem;
    old = *l;
  }
  elems = realloc(elems, (count + 1) * sizeof(void *));
  elems[count] = NULL;
  *worked = count > 0;
  return elems;
}
// RULE type
void *parse_type(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_type_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_type_c1(&rule_cpy, &rule_worked);
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
    lexer_t old = *l;
    void *elem = parse_stmt(l, &rule_worked);
    if (!rule_worked) {
      *l = old;
      break;
    }
    if (count >= cap) {
      cap *= 2;
      elem = realloc(elem, cap * sizeof(void *));
    }
    elems[count++] = elem;
    old = *l;
  }
  elems = realloc(elems, (count + 1) * sizeof(void *));
  elems[count] = NULL;
  *worked = count > 0;
  return elems;
}
// RULE compound
void *parse_compound(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_compound_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_compound_c1(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

void *parse_program_list(lexer_t *l, int *worked) {
  int rule_worked = 0;
  size_t cap = 16;
  size_t count = 0;
  void **elems = malloc(sizeof(void *) * cap);
  while (1) {
    lexer_t old = *l;
    void *elem = parse_decl(l, &rule_worked);
    if (!rule_worked) {
      *l = old;
      break;
    }
    if (count >= cap) {
      cap *= 2;
      elem = realloc(elem, cap * sizeof(void *));
    }
    elems[count++] = elem;
    old = *l;
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

// RULE fundef_letless
void *parse_fundef_letless(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_fundef_letless_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_fundef_letless_c1(&rule_cpy, &rule_worked);
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

// RULE vardef_letless
void *parse_vardef_letless(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_vardef_letless_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_vardef_letless_c1(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE vardef
void *parse_vardef(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_vardef_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE ct_cte
void *parse_ct_cte(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_ct_cte_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE access_spec
void *parse_access_spec(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_access_spec_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_access_spec_c1(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE abstract_opt
void *parse_abstract_opt(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_abstract_opt_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_abstract_opt_c1(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE static_opt
void *parse_static_opt(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_static_opt_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_static_opt_c1(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

void *parse_class_body(lexer_t *l, int *worked) {
  int rule_worked = 0;
  size_t cap = 16;
  size_t count = 0;
  void **elems = malloc(sizeof(void *) * cap);
  while (1) {
    lexer_t old = *l;
    void *elem = parse_class_body_item(l, &rule_worked);
    if (!rule_worked) {
      break;
    }
    if (count >= cap) {
      cap *= 2;
      elem = realloc(elem, cap * sizeof(void *));
    }
    elems[count++] = elem;
    old = *l;
    (void)parse_token_lexeme(l, &rule_worked, SV(","));
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
// RULE class_body_item
void *parse_class_body_item(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_class_body_item_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_class_body_item_c1(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE class_decl
void *parse_class_decl(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_class_decl_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE if_statement
void *parse_if_statement(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_if_statement_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  rule_cpy = *l;
  rule_res = parse_if_statement_c1(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE while_stmt
void *parse_while_stmt(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_while_stmt_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE assignement
void *parse_assignement(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_assignement_c0(&rule_cpy, &rule_worked);
  if (rule_worked) {
    *worked = 1;
    *l = rule_cpy;
    return rule_res;
  }
  return NULL;
}

// RULE return
void *parse_return(lexer_t *l, int *worked) {
  *worked = 0;
  int rule_worked = 0;
  void *rule_res = NULL;
  lexer_t rule_cpy = *l;
  rule_res = parse_return_c0(&rule_cpy, &rule_worked);
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
  void *elem_2 = parse_type(l, worked);
  if (!*worked) {
    return NULL;
  }

  ast_t *id = elem_0;
  ast_t *type = elem_2;
  return new_param(id, type);
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
  void *elem_0 = parse_literal(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_leaf_c2(lexer_t *l, int *worked) {
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
void *parse_stmt_c1(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_compound(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_stmt_c2(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_vardef(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_stmt_c3(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_if_statement(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_stmt_c4(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_while_stmt(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_stmt_c5(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_assignement(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_stmt_c6(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_return(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_decl_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_ct_cte(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_decl_c1(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_fundef(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_decl_c2(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_vardef(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_decl_c3(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_class_decl(l, worked);
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
void *parse_type_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_identifier(l, worked);
  if (!*worked) {
    return NULL;
  }
  void *elem_1 = parse_starlist(l, worked);
  if (!*worked) {
    return NULL;
  }

  ast_t *iden = elem_0;
  token_t **starlist = elem_1;
  size_t count = 0;
  while (starlist[count]) {
    count++;
  }

  for (size_t i = 0; i < count; ++i) {
    free(starlist[i]);
  }
  free(starlist);
  token_t tok = iden->as.identifier.tok;
  free_ast(iden);
  return new_type(tok, count);
}
void *parse_type_c1(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_identifier(l, worked);
  if (!*worked) {
    return NULL;
  }

  ast_t *iden = elem_0;
  token_t tok = iden->as.identifier.tok;
  free_ast(iden);
  return new_type(tok, 0);
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
void *parse_compound_c0(lexer_t *l, int *worked) {
  *worked = 0;
  free(parse_token_lexeme(l, worked, SV("{")));
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("}")));
  if (!*worked) {
    return NULL;
  }

  ast_t **res = malloc(sizeof(ast_t *));
  *res = NULL;
  return new_compound(res);
}
void *parse_compound_c1(lexer_t *l, int *worked) {
  *worked = 0;
  free(parse_token_lexeme(l, worked, SV("{")));
  if (!*worked) {
    return NULL;
  }
  void *elem_1 = parse_stmt_list(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("}")));
  if (!*worked) {
    return NULL;
  }
  return new_compound(elem_1);
}
void *parse_program_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_program_list(l, worked);
  if (!*worked) {
    return NULL;
  }

  return new_compound(elem_0);
}
void *parse_fundef_letless_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_identifier(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("(")));
  if (!*worked) {
    return NULL;
  }
  void *elem_2 = parse_arglist(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV(")")));
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV(":")));
  if (!*worked) {
    return NULL;
  }
  void *elem_5 = parse_type(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("=>")));
  if (!*worked) {
    return NULL;
  }
  void *elem_7 = parse_compound(l, worked);
  if (!*worked) {
    return NULL;
  }

  ast_t *id = elem_0;
  ast_t *type = elem_5;
  tmp_param_t **tmp_arglist = elem_2;
  ast_t *stmt_list = elem_7;
  token_t tok = id->as.identifier.tok;
  free_ast(id);
  return new_fundef_from_parser(tok, tmp_arglist, stmt_list, type);
}
void *parse_fundef_letless_c1(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_identifier(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("(")));
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV(")")));
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV(":")));
  if (!*worked) {
    return NULL;
  }
  void *elem_4 = parse_type(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("=>")));
  if (!*worked) {
    return NULL;
  }
  void *elem_6 = parse_compound(l, worked);
  if (!*worked) {
    return NULL;
  }

  ast_t *id = elem_0;
  ast_t *type = elem_4;
  tmp_param_t **tmp_arglist = malloc(sizeof(void *));
  tmp_arglist[0] = NULL;
  ast_t *stmt_list = elem_6;
  token_t tok = id->as.identifier.tok;
  free_ast(id);
  return new_fundef_from_parser(tok, tmp_arglist, stmt_list, type);
}
void *parse_fundef_c0(lexer_t *l, int *worked) {
  *worked = 0;
  free(parse_token_lexeme(l, worked, SV("let")));
  if (!*worked) {
    return NULL;
  }
  void *elem_1 = parse_fundef_letless(l, worked);
  if (!*worked) {
    return NULL;
  }
  return elem_1;
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
  void *elem_0 = parse_token_lexeme(l, worked, SV("!"));
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_vardef_letless_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_identifier(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV(":")));
  if (!*worked) {
    return NULL;
  }
  void *elem_2 = parse_type(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("=>")));
  if (!*worked) {
    return NULL;
  }
  void *elem_4 = parse_expr(l, worked);
  if (!*worked) {
    return NULL;
  }

  ast_t *iden = elem_0;
  token_t tok = iden->as.identifier.tok;
  free(iden);
  return new_vardef(tok, elem_2, elem_4);
}
void *parse_vardef_letless_c1(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_identifier(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV(":")));
  if (!*worked) {
    return NULL;
  }
  void *elem_2 = parse_type(l, worked);
  if (!*worked) {
    return NULL;
  }

  ast_t *iden = elem_0;
  token_t tok = iden->as.identifier.tok;
  free(iden);
  return new_vardef(tok, elem_2, NULL);
}
void *parse_vardef_c0(lexer_t *l, int *worked) {
  *worked = 0;
  free(parse_token_lexeme(l, worked, SV("let")));
  if (!*worked) {
    return NULL;
  }
  void *elem_1 = parse_vardef_letless(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV(";")));
  if (!*worked) {
    return NULL;
  }
  return elem_1;
}
void *parse_ct_cte_c0(lexer_t *l, int *worked) {
  *worked = 0;
  free(parse_token_lexeme(l, worked, SV("@const")));
  if (!*worked) {
    return NULL;
  }
  void *elem_1 = parse_token_kind(l, worked, IDENTIFIER);
  if (!*worked) {
    return NULL;
  }
  void *elem_2 = parse_expr(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV(";")));
  if (!*worked) {
    return NULL;
  }

  token_t *iden_ptr = elem_1;
  token_t iden = *iden_ptr;
  free(iden_ptr);
  ast_t *expr = elem_2;
  return new_ct_cte(iden, expr);
}
void *parse_access_spec_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_token_lexeme(l, worked, SV("public"));
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_access_spec_c1(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_token_lexeme(l, worked, SV("private"));
  if (!*worked) {
    return NULL;
  }
  return elem_0;
}
void *parse_abstract_opt_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_token_lexeme(l, worked, SV("abstract"));
  if (!*worked) {
    return NULL;
  }

  return elem_0;
}
void *parse_abstract_opt_c1(lexer_t *l, int *worked) {
  *worked = 0;

  *worked = 1;
  (void)l;
  return NULL;
}
void *parse_static_opt_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_token_lexeme(l, worked, SV("static"));
  if (!*worked) {
    return NULL;
  }

  return elem_0;
}
void *parse_static_opt_c1(lexer_t *l, int *worked) {
  *worked = 0;

  *worked = 1;
  (void)l;
  return NULL;
}
void *parse_class_body_item_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_abstract_opt(l, worked);
  if (!*worked) {
    return NULL;
  }
  void *elem_1 = parse_access_spec(l, worked);
  if (!*worked) {
    return NULL;
  }
  void *elem_2 = parse_static_opt(l, worked);
  if (!*worked) {
    return NULL;
  }
  void *elem_3 = parse_fundef_letless(l, worked);
  if (!*worked) {
    return NULL;
  }

  token_t *access_spec_ptr = elem_1;
  token_t access_spec = *access_spec_ptr;
  uintptr_t abstract_opt = (uintptr_t)elem_0;
  uintptr_t static_opt = (uintptr_t)elem_2;
  free(access_spec_ptr);
  free(elem_0);
  free(elem_2);
  ast_t *fundef = elem_3;
  return new_method(fundef, access_spec, abstract_opt != 0, static_opt != 0);
}
void *parse_class_body_item_c1(lexer_t *l, int *worked) {
  *worked = 0;
  free(parse_access_spec(l, worked));
  if (!*worked) {
    return NULL;
  }
  free(parse_static_opt(l, worked));
  if (!*worked) {
    return NULL;
  }
  void *elem_2 = parse_vardef_letless(l, worked);
  if (!*worked) {
    return NULL;
  }

  return elem_2;
}
void *parse_class_decl_c0(lexer_t *l, int *worked) {
  *worked = 0;
  free(parse_token_lexeme(l, worked, SV("class")));
  if (!*worked) {
    return NULL;
  }
  void *elem_1 = parse_identifier(l, worked);
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
  void *elem_4 = parse_class_body(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("}")));
  if (!*worked) {
    return NULL;
  }

  ast_t *iden = elem_1;
  token_t name = iden->as.identifier.tok;
  free_ast(iden);
  ast_t **body = elem_4;
  size_t count = 0;
  while (body[count]) {
    count++;
  }
  return new_class(name, count, body);
}
void *parse_if_statement_c0(lexer_t *l, int *worked) {
  *worked = 0;
  free(parse_token_lexeme(l, worked, SV("if")));
  if (!*worked) {
    return NULL;
  }
  void *elem_1 = parse_expr(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("=>")));
  if (!*worked) {
    return NULL;
  }
  void *elem_3 = parse_stmt(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("else")));
  if (!*worked) {
    return NULL;
  }
  void *elem_5 = parse_stmt(l, worked);
  if (!*worked) {
    return NULL;
  }

  ast_t *cond = elem_1;
  ast_t *stmt1 = elem_3;
  ast_t *stmt2 = elem_5;
  return new_if_stmt(cond, stmt1, stmt2);
}
void *parse_if_statement_c1(lexer_t *l, int *worked) {
  *worked = 0;
  free(parse_token_lexeme(l, worked, SV("if")));
  if (!*worked) {
    return NULL;
  }
  void *elem_1 = parse_expr(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("=>")));
  if (!*worked) {
    return NULL;
  }
  void *elem_3 = parse_stmt(l, worked);
  if (!*worked) {
    return NULL;
  }

  ast_t *cond = elem_1;
  ast_t *stmt1 = elem_3;
  return new_if_stmt(cond, stmt1, NULL);
}
void *parse_while_stmt_c0(lexer_t *l, int *worked) {
  *worked = 0;
  free(parse_token_lexeme(l, worked, SV("while")));
  if (!*worked) {
    return NULL;
  }
  void *elem_1 = parse_expr(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("=>")));
  if (!*worked) {
    return NULL;
  }
  void *elem_3 = parse_stmt(l, worked);
  if (!*worked) {
    return NULL;
  }

  return new_while_stmt(elem_1, elem_3);
}
void *parse_assignement_c0(lexer_t *l, int *worked) {
  *worked = 0;
  void *elem_0 = parse_expr(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV("=>")));
  if (!*worked) {
    return NULL;
  }
  void *elem_2 = parse_expr(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV(";")));
  if (!*worked) {
    return NULL;
  }

  return new_assignement(elem_0, elem_2);
}
void *parse_return_c0(lexer_t *l, int *worked) {
  *worked = 0;
  free(parse_token_lexeme(l, worked, SV("return")));
  if (!*worked) {
    return NULL;
  }
  void *elem_1 = parse_expr(l, worked);
  if (!*worked) {
    return NULL;
  }
  free(parse_token_lexeme(l, worked, SV(";")));
  if (!*worked) {
    return NULL;
  }

  return new_return(elem_1);
}
