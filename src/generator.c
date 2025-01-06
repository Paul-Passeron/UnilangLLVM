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
  type_t t = {0, 0, 0, strdup(name), type, NULL, NULL};
  add_actual_type(g, t);
}

void add_function_ast(generator_t *g, ast_t *fun) {
  if (g->funcs_count >= g->funcs_cap) {
    g->funcs_cap *= 2;
    ast_t **new_funcs = malloc(sizeof(ast_t *) * g->funcs_cap);
    memcpy(new_funcs, g->funcs, sizeof(ast_t *) * g->funcs_count);
    free(g->funcs);
    g->funcs = new_funcs;
  }
  g->funcs[g->funcs_count++] = fun;
}

void init_functions(generator_t *g, ast_t *program) {
  ast_program_t prog = program->as.program;
  for (size_t i = 0; i < prog.elem_count; ++i) {
    ast_t *decl = prog.elems[i];
    if (decl->kind == AST_FUNDEF) {
      add_function_ast(g, decl);
    }
  }
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
  g->named_values_vals = malloc(sizeof(LLVMValueRef) * INIT_NAMED_VALUES_CAP);
  g->named_values_names = malloc(sizeof(char *) * INIT_NAMED_VALUES_CAP);
  g->named_values_types = malloc(sizeof(type_t) * INIT_NAMED_VALUES_CAP);
  g->named_values_count = 0;

  g->funcs_cap = INIT_FUNCS_CAP;
  g->funcs = malloc(sizeof(ast_t *) * g->funcs_cap);
  g->funcs_count = 0;

  addBuiltinTypes(g);
}

ast_t *get_function_ast_from_name(generator_t *g, char *fname) {
  size_t l = strlen(fname);
  string_view_t name = {fname, l};
  //
  for (size_t i = 0; i < g->funcs_count; ++i) {
    string_view_t sv = g->funcs[i]->as.fundef.name.lexeme;
    if (sv_eq(name, sv)) {
      return g->funcs[i];
    }
  }
  // TODO: actual error handling
  printf("Function " SF " is not declared in the current scope.\n", SA(name));
  exit(1);
  return NULL;
}

int get_named_values_scope(generator_t *g) { return g->named_values_count; }

void reset_named_values_to_scope(generator_t *g, int scope) {
  for (int i = g->named_values_count - 1; i >= scope; i--) {
    free(g->named_values_names[i]);
  }
  g->named_values_count = scope;
}

void push_named_value(generator_t *g, LLVMValueRef value, const char *name,
                      type_t t) {
  if (g->named_values_count >= g->named_values_cap) {
    LLVMValueRef *new_values =
        malloc(g->named_values_cap * 2 * sizeof(LLVMValueRef));
    memcpy(new_values, g->named_values_vals,
           g->named_values_cap * sizeof(LLVMValueRef));
    type_t *new_types = malloc(g->named_values_cap * 2 * sizeof(type_t));
    memcpy(new_types, g->named_values_types,
           g->named_values_cap * sizeof(type_t));
    char **new_names = malloc(g->named_values_cap * 2 * sizeof(char *));
    memcpy(new_names, g->named_values_names,
           g->named_values_cap * sizeof(char *));
    free(g->named_values_vals);
    free(g->named_values_names);
    g->named_values_vals = new_values;
    g->named_values_types = new_types;
    g->named_values_names = new_names;
    g->named_values_cap *= 2;
  }
  g->named_values_vals[g->named_values_count] = value;
  g->named_values_names[g->named_values_count] = strdup(name);
  g->named_values_types[g->named_values_count] = t;
  g->named_values_count++;
}

bool are_types_equal(type_t a, type_t b) {
  if (a.name != NULL && b.name != NULL) {
    return strcmp(a.name, b.name) == 0;
  }
  return LLVMSizeOf(a.type) == LLVMSizeOf(b.type) && a.is_ptr == b.is_ptr;
}

void overwrite_pushed_value(generator_t *g, LLVMValueRef new_value, char *name,
                            type_t t) {
  bool worked = false;
  for (int i = g->named_values_count - 1; i >= 0; --i) {
    if (strcmp(g->named_values_names[i], name) == 0 &&
        are_types_equal(t, g->named_values_types[i])) {
      g->named_values_vals[i] = new_value;
      worked = true;
      break;
    }
  }
  if (!worked) {
    push_named_value(g, new_value, name, t);
  }
}

type_t add_class_type(generator_t *g, const char *name, LLVMTypeRef type,
                      ast_t *decl) {
  type_t t = {0, 1, 1, strdup(name), type, decl, NULL};
  add_actual_type(g, t);
  return t;
}

void add_type(generator_t *g, const char *name, LLVMTypeRef type) {
  type_t t = {0, 1, 0, strdup(name), type, NULL, NULL};
  add_actual_type(g, t);
}
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
  type_t *pointed_to = malloc(sizeof(type_t));
  *pointed_to = t;
  return (type_t){1, 0, 0, strdup("ptr"), ptr, NULL, pointed_to};
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
    } else {
      printf("UNDEFINED TYPE:\n");
      dump_ast(type);
      exit(71);
    }
    free(n);
    return res;
  }
  ast_t modified = *type;
  modified.as.type.ptr_n = 0;
  type_t original = get_type_from_ast(g, &modified);
  for (size_t i = 0; i < type->as.type.ptr_n; ++i) {
    original = get_ptr_of(original);
  }
  return original;
}

bool is_method_constructor(generator_t *g, ast_t *classdef, ast_t *m) {
  (void)g;
  ast_class_t clazz = classdef->as.clazz;
  ast_method_t method = m->as.method;
  return sv_eq(method.fdef->as.fundef.name.lexeme, clazz.name.lexeme);
}

void generate_method(generator_t *g, ast_t *method, type_t class) {
  (void)g;
  (void)method;
  (void)class;
  ast_fundef_t f = method->as.method.fdef->as.fundef;
  string_view_t svn = f.name.lexeme;
  char *name = malloc(svn.length + strlen(class.name) + 2);
  if (name == NULL) {
    printf("Memory allocation failed\n");
    exit(1);
  }
  memset(name, 0, svn.length + strlen(class.name) + 2);
  strcpy(name, class.name);
  strcpy(name + strlen(class.name), "_");
  strncpy(name + strlen(class.name) + 1, svn.contents, svn.length);
  type_t return_type = get_type_from_ast(g, f.return_type);
  LLVMTypeRef r = return_type.type;
  type_t *types = malloc(sizeof(type_t) * (f.param_count + 1));
  LLVMTypeRef *param_types = malloc(sizeof(LLVMTypeRef) * (f.param_count + 1));
  type_t self_type = get_ptr_of(class);
  types[0] = self_type;           // self
  param_types[0] = types[0].type; // self
  for (size_t i = 0; i < f.param_count; ++i) {
    types[i + 1] = get_type_from_ast(g, f.param_types[i]);
    param_types[i + 1] = types[i + 1].type;
  }
  LLVMTypeRef func_type =
      LLVMFunctionType(r, param_types, f.param_count + 1, 0);
  LLVMValueRef func = LLVMAddFunction(g->module, name, func_type);
  g->current_function = func;
  g->current_function_type = func_type;

  int scope = get_named_values_scope(g);

  LLVMValueRef self = LLVMGetParam(func, 0);
  char *self_name = "self";
  push_named_value(g, self, self_name, self_type);

  for (size_t i = 1; i < f.param_count + 1; i++) {
    LLVMValueRef param = LLVMGetParam(func, i);
    char *name = strndup(f.param_names[i - 1].lexeme.contents,
                         f.param_names[i - 1].lexeme.length);
    push_named_value(g, param, name, types[i]);
  }

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(func, "entry");
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

void generate_constructor(generator_t *g, ast_t *constructor, type_t class) {
  (void)g;
  (void)constructor;
  (void)class;
  printf("TODO: generate constructor\n");
  exit(71);
}
void generate_class(generator_t *g, ast_t *decl) {
  // Don't care about access specifier for the moment
  // Don't care about static vars / funcs for the moment
  ast_class_t c = decl->as.clazz;
  size_t member_vars_fields_count = 0;
  for (size_t i = 0; i < c.field_count; ++i) {
    ast_member_t m = c.fields[i]->as.member;
    if (m.var->kind == AST_VARDEF) {
      member_vars_fields_count++;
    }
  }
  ast_t **member_vars = malloc(member_vars_fields_count * sizeof(ast_t));
  member_vars_fields_count = 0;
  for (size_t i = 0; i < c.field_count; ++i) {
    if (c.fields[i]->kind == AST_MEMBER) {
      member_vars[member_vars_fields_count++] = c.fields[i]->as.member.var;
    }
  }
  type_t *types = malloc(member_vars_fields_count * sizeof(type_t));
  for (size_t i = 0; i < member_vars_fields_count; ++i) {
    ast_vardef_t v = member_vars[i]->as.vardef;
    types[i] = get_type_from_ast(g, v.type);
  }
  LLVMTypeRef *ts = malloc(member_vars_fields_count * sizeof(LLVMTypeRef));
  for (size_t i = 0; i < member_vars_fields_count; ++i) {
    ts[i] = types[i].type;
  }
  char *name = strndup(c.name.lexeme.contents, c.name.lexeme.length);
  LLVMTypeRef class_type = LLVMStructCreateNamed(g->context, name);
  LLVMStructSetBody(class_type, ts, member_vars_fields_count, 0);
  type_t ct = add_class_type(g, name, class_type, decl);
  free(name);
  for (size_t i = 0; i < c.field_count; ++i) {
    if (c.fields[i]->kind == AST_METHOD) {
      ast_t *m = c.fields[i];
      if (is_method_constructor(g, decl, m)) {
        generate_constructor(g, m, ct);
      } else {
        generate_method(g, m, ct);
      }
    }
  }

  printf("Here !\n");
}

void generate_program(generator_t *g, ast_t *program) {
  init_functions(g, program);
  for (size_t i = 0; i < program->as.program.elem_count; i++) {
    ast_t *decl = program->as.program.elems[i];
    if (decl->kind == AST_FUNDEF) {
      generate_function(g, decl);
    } else if (decl->kind == AST_CLASS) {
      generate_class(g, decl);
    } else {
      printf("TODO: decl->kind = %d\n", decl->kind);
      exit(15);
    }
  }
}

int get_named_value(generator_t *g, char *name) {
  for (size_t i = 0; i < g->named_values_count; ++i) {
    int index = g->named_values_count - 1 - i;
    if (strcmp(g->named_values_names[index], name) == 0) {
      return index;
    }
  }
  return -1;
}

LLVMTypeRef generate_funtype(generator_t *g, ast_t *fun) {
  ast_fundef_t f = fun->as.fundef;
  type_t return_type = get_type_from_ast(g, f.return_type);
  LLVMTypeRef r = return_type.type;
  type_t *types = malloc(sizeof(type_t) * f.param_count);
  LLVMTypeRef *param_types = malloc(sizeof(LLVMTypeRef) * f.param_count);
  for (size_t i = 0; i < f.param_count; ++i) {
    types[i] = get_type_from_ast(g, f.param_types[i]);
    param_types[i] = types[i].type;
  }
  LLVMTypeRef fun_type = LLVMFunctionType(r, param_types, f.param_count, false);
  return fun_type;
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

  for (size_t i = 0; i < f.param_count; i++) {
    LLVMValueRef param = LLVMGetParam(func, i);
    char *name = strndup(f.param_names[i].lexeme.contents,
                         f.param_names[i].lexeme.length);
    type_t t = get_type_from_ast(g, f.param_types[i]);
    push_named_value(g, param, name, t);
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
  case MINUS: {
    return LLVMBuildSub(g->builder, left, right, "");
  } break;
  case MULT: {
    return LLVMBuildMul(g->builder, left, right, "");
  }
  default: {
    printf("Unsupported operator: " SF "\n", SA(op.lexeme));
    exit(1);
  }
  }
}

LLVMValueRef generate_unop(generator_t *g, token_t op, LLVMValueRef value) {
  switch (op.kind) {
  case MINUS: {
    return LLVMBuildNeg(g->builder, value, "");
  } break;
  default: {
    printf("Unsupported unary operator: " SF "\n", SA(op.lexeme));
    exit(1);
  } break;
  }
}

bool are_types_castable(generator_t *g, type_t a, type_t b) {
  if (are_types_equal(a, b))
    return true;
  if (is_integer_type(g, a) && is_integer_type(g, b))
    return true;
  if (a.is_ptr == b.is_ptr)
    return true;
  return false;
}

ast_t *get_named_function(generator_t *g, const char *name) {
  for (size_t i = 0; i < g->funcs_count; ++i) {
    ast_t *fun = g->funcs[i];
    string_view_t funname = fun->as.fundef.name.lexeme;
    if (strncmp(name, funname.contents, funname.length) == 0) {
      return fun;
    }
  }
  return NULL;
}

char *get_function_name(generator_t *g, ast_t *called) {
  switch (called->kind) {
  case AST_IDENTIFIER: {
    token_t name = called->as.identifier.tok;
    char *cstrname = strndup(name.lexeme.contents, name.lexeme.length);
    return cstrname;
  }
  case AST_BINOP: {
    ast_binop_t bin = called->as.binop;
    switch (bin.op.kind) {
    case ACCESS: {
      type_t t = get_type_from_expr(g, bin.lhs);
      printf("TYPE NAME IS %s and is %sclass\n", t.name,
             t.is_class ? "" : "not a ");
      // TODO: check that class had method
      if (t.is_class) {

        if (bin.rhs->kind != AST_IDENTIFIER) {
          printf("TODO...\n");
          exit(1);
        }
        token_t name = bin.rhs->as.identifier.tok;
        char *fun_name = malloc(name.lexeme.length + strlen(t.name) + 2);
        *fun_name = 0;
        strcat(fun_name, t.name);
        strcat(fun_name, "_");
        strncat(fun_name, name.lexeme.contents, name.lexeme.length);
        return fun_name;
      }
    }
    default: {
      break;
    }
    }
  }
  default:
    break;
  }
  printf("Could not find function:\n");
  dump_ast(called);
  printf("\n");
  exit(1);
}

LLVMTypeRef get_funcall_type(generator_t *g, ast_t *called) {
  switch (called->kind) {
  case AST_IDENTIFIER: {
    token_t name = called->as.identifier.tok;
    char *cstrname = strndup(name.lexeme.contents, name.lexeme.length);
    ast_t *func = get_named_function(g, cstrname);
    LLVMTypeRef funtype = generate_funtype(g, func);
    return funtype;
  }
  case AST_BINOP: {
    ast_binop_t bin = called->as.binop;
    switch (bin.op.kind) {
    case ACCESS: {
      printf("SHOULD FAIL HERE\n");
      exit(54);
    }
    default: {
      break;
    }
    }
  }
  default:
    break;
  }
  printf("Could not find function:\n");
  dump_ast(called);
  printf("\n");
  exit(1);
}

LLVMValueRef generate_funcall(generator_t *g, ast_t *fcall) {
  ast_t *called = fcall->as.funcall.called;

  char *cstrname = get_function_name(g, called);
  ast_t *func = get_named_function(g, cstrname);
  if (func == NULL) {
    printf("Undefined function: %s\n", cstrname);
    exit(1);
  }
  if (func->as.fundef.param_count != fcall->as.funcall.arg_count) {
    int cargs = fcall->as.funcall.arg_count;
    int dargs = func->as.fundef.param_count;
    printf("Function \'%s\' expects %d arguments but %s%d were given\n",
           cstrname, dargs, dargs > cargs ? "only" : "", cargs);
    exit(1);
  }
  LLVMValueRef *args =
      malloc(sizeof(LLVMValueRef) * fcall->as.funcall.arg_count);
  for (size_t i = 0; i < fcall->as.funcall.arg_count; ++i) {
    type_t t;
    t = get_type_from_ast(g, func->as.fundef.param_types[i]);
    LLVMValueRef arg =
        generate_expression_with_type(g, fcall->as.funcall.args[i], t);
    args[i] = arg;
  }
  LLVMValueRef func_ptr = LLVMGetNamedFunction(g->module, cstrname);
  LLVMTypeRef funtype = generate_funtype(g, func);
  LLVMValueRef res = LLVMBuildCall2(g->builder, funtype, func_ptr, args,
                                    fcall->as.funcall.arg_count, "");
  free(cstrname);
  free(args);
  return res;
}

LLVMValueRef generate_strlit(generator_t *g, ast_t *strlit) {
  (void)g;
  token_t tmp = strlit->as.stringlit.tok;
  string_view_t actual;
  actual.contents = tmp.lexeme.contents + 1;
  actual.length = tmp.lexeme.length - 1;
  char *cstr = strndup(actual.contents, actual.length);
  LLVMTypeRef char_type = get_type_from_name(g, "char").type;
  LLVMValueRef str = LLVMAddGlobal(
      g->module, LLVMArrayType(char_type, actual.length + 1), cstr);
  LLVMSetInitializer(str, LLVMConstString(cstr, actual.length, 0));
  free(cstr);
  return LLVMBuildBitCast(g->builder, str, LLVMPointerType(char_type, 0), "");
}

type_t get_type_from_expr(generator_t *g, ast_t *expr) {
  switch (expr->kind) {
  case AST_IDENTIFIER: {
    string_view_t sv = expr->as.identifier.tok.lexeme;
    char *name = strndup(sv.contents, sv.length);
    int i = get_named_value(g, name);
    type_t res = g->named_values_types[i];
    free(name);
    return res;
  }
  case AST_BINOP: {
    if (expr->as.binop.op.kind == ACCESS) {
      printf("TODO: get type from access\n");
      exit(54);
    }
    type_t left = get_type_from_expr(g, expr->as.binop.lhs);
    type_t right = get_type_from_expr(g, expr->as.binop.rhs);
    printf("\nl: ");
    LLVMDumpType(left.type);
    printf("\nr: ");
    LLVMDumpType(right.type);
    if (LLVMSizeOf(left.type) < LLVMSizeOf(right.type)) {
      return right;
    }
    return left;
  }
  default: {
    printf("Could not get type from expression:\n");
    dump_ast(expr);
    printf("\n");
    exit(1);
  }
  }
}

type_t dereference_type(generator_t *g, type_t t) {
  (void)g;
  if (!t.is_ptr) {
    printf("Cannot dereference non-pointer type\n");
    exit(13);
  }
  return *t.pointed_to;
}

LLVMValueRef generate_access_value(generator_t *g, ast_t *expr, type_t t) {
  LLVMValueRef lhs = generate_expression(g, expr->as.binop.lhs);
  if (!t.is_class) {
    printf("Cannot access member from non-class type...\n");
    exit(4);
  }
  ast_class_t cdef = t.classdecl->as.clazz;
  if (expr->as.binop.rhs->kind != AST_IDENTIFIER) {
    printf("Cannot access member with non-identifier index...\n");
    exit(5);
  }
  string_view_t sv = expr->as.binop.rhs->as.identifier.tok.lexeme;
  int found = 0;
  int res = 0;
  for (size_t i = 0; i < cdef.field_count; ++i) {
    ast_t *current = cdef.fields[i];
    if (current->kind == AST_MEMBER) {
      string_view_t sv2 = current->as.member.var->as.vardef.name.lexeme;
      if (sv_eq(sv, sv2)) {
        found = true;
        break;
      }
      res++;
    }
  }
  if (!found) {
    printf("Member " SF " not found in class %s...\n", SA(sv), t.name);
    exit(8);
  }
  LLVMValueRef ptr = LLVMBuildAlloca(g->builder, t.type, "ptr");
  LLVMBuildStore(g->builder, lhs, ptr);
  type_t res_type =
      get_type_from_ast(g, cdef.fields[res]->as.member.var->as.vardef.type);
  LLVMValueRef res_ptr =
      LLVMBuildStructGEP2(g->builder, t.type, ptr, res, "gep");
  return LLVMBuildLoad2(g->builder, res_type.type, res_ptr, "load");
}

LLVMValueRef generate_access_ptr(generator_t *g, ast_t *expr, type_t t) {
  LLVMValueRef lhs = generate_expression(g, expr->as.binop.lhs);
  type_t class_type = dereference_type(g, t);
  ast_class_t cdef = class_type.classdecl->as.clazz;
  if (expr->as.binop.rhs->kind != AST_IDENTIFIER) {
    printf("Cannot access member with non-identifier index...\n");
    exit(5);
  }
  string_view_t sv = expr->as.binop.rhs->as.identifier.tok.lexeme;
  int found = 0;
  int res = 0;
  for (size_t i = 0; i < cdef.field_count; ++i) {
    ast_t *current = cdef.fields[i];
    if (current->kind == AST_MEMBER) {
      string_view_t sv2 = current->as.member.var->as.vardef.name.lexeme;
      if (sv_eq(sv, sv2)) {
        found = true;
        break;
      }
      res++;
    }
  }
  if (!found) {
    printf("Member " SF " not found in class %s...\n", SA(sv), t.name);
    exit(8);
  }
  type_t res_type =
      get_type_from_ast(g, cdef.fields[res]->as.member.var->as.vardef.type);
  LLVMValueRef res_ptr =
      LLVMBuildStructGEP2(g->builder, class_type.type, lhs, res, "gep");
  return LLVMBuildLoad2(g->builder, res_type.type, res_ptr, "load");
}

LLVMValueRef generate_access(generator_t *g, ast_t *expr) {
  type_t t = get_type_from_expr(g, expr->as.binop.lhs);
  if (t.is_ptr) {
    return generate_access_ptr(g, expr, t);
  }
  return generate_access_value(g, expr, t);
}

LLVMValueRef generate_expression_with_type(generator_t *g, ast_t *expr,
                                           type_t type) {
  (void)g;
  switch (expr->kind) {
  case AST_INTLIT: {
    return generate_intlit(g, expr->as.intlit.tok, type);
  } break;
  case AST_STRINGLIT: {
    return generate_strlit(g, expr);
  } break;
  case AST_BINOP: {
    if (expr->as.binop.op.kind == ACCESS) {
      return generate_access(g, expr);
    }
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
    type_t original_type = g->named_values_types[named_index];
    LLVMValueRef raw_val = g->named_values_vals[named_index];
    if (are_types_equal(original_type, type))
      return raw_val;
    if (!are_types_castable(g, original_type, type)) {
      printf("Incompatible types: ");
      fflush(stdout);
      LLVMDumpType(original_type.type);
      printf(" cannot be implicitely converted to ");
      fflush(stdout);
      LLVMDumpType(type.type);
      exit(1);
    }
    if (is_integer_type(g, type)) {
      return LLVMBuildIntCast(g->builder, raw_val, type.type, "intcast");
    }

    return LLVMBuildCast(g->builder, LLVMBitCast, raw_val, type.type, "cast");
  } break;
  case AST_UNOP: {
    LLVMValueRef operand =
        generate_expression_with_type(g, expr->as.unop.operand, type);
    return generate_unop(g, expr->as.unop.op, operand);
  } break;
  case AST_FUNCALL: {
    return generate_funcall(g, expr);
  } break;
  default: {
    printf("Unsupported expression %d\n", expr->kind);
    exit(1);
  } break;
  }
  exit(70);
}

LLVMValueRef generate_expression(generator_t *g, ast_t *expr) {
  type_t t = get_type_from_expr(g, expr);
  return generate_expression_with_type(g, expr, t);
}

void generate_return(generator_t *g, ast_t *expr) {
  if (expr == NULL) {
    LLVMBuildRetVoid(g->builder);
    return;
  }
  LLVMTypeRef current_fun_type = g->current_function_type;
  LLVMTypeRef current_ret_type = LLVMGetReturnType(current_fun_type);
  type_t ret_type = from_llvm(current_ret_type);
  LLVMValueRef expr_value = generate_expression_with_type(g, expr, ret_type);
  LLVMBuildRet(g->builder, expr_value);
}

LLVMValueRef get_default_value_for_type(generator_t *g, type_t t) {
  (void)g;
  (void)t;
  printf("TODO: get_default_value_for_type\n");
  exit(8);
}

void generate_vardef(generator_t *g, ast_t *stmt) {
  ast_vardef_t v = stmt->as.vardef;
  LLVMValueRef val;
  type_t t = get_type_from_ast(g, v.type);
  if (v.value == NULL) {
    val = get_default_value_for_type(g, t);
  } else {
    val = generate_expression_with_type(g, v.value, t);
  }
  char *name = strndup(v.name.lexeme.contents, v.name.lexeme.length);
  push_named_value(g, val, name, t);
}

void generate_store(generator_t *g, ast_t *lhs, LLVMValueRef val) {
  (void)g;
  (void)lhs;
  (void)val;
  printf("TODO: generate_store\n");
  exit(2);
}

void generate_assign(generator_t *g, ast_t *stmt) {
  ast_assign_t as = stmt->as.assign;
  if (as.lhs->kind != AST_IDENTIFIER) {
    LLVMValueRef val =
        generate_expression_with_type(g, as.rhs, get_type_from_expr(g, as.rhs));
    generate_store(g, as.lhs, val);
    return;
  }
  string_view_t s = as.lhs->as.identifier.tok.lexeme;
  char *name = strndup(s.contents, s.length);
  int i = get_named_value(g, name);
  type_t t = g->named_values_types[i];
  LLVMValueRef val = generate_expression_with_type(g, as.rhs, t);
  overwrite_pushed_value(g, val, name, t);
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
  } break;
  case AST_VARDEF: {
    generate_vardef(g, stmt);
  } break;
  case AST_ASSIGN: {
    generate_assign(g, stmt);

  } break;
  default: {
    // could not do that ?
    (void)generate_expression_with_type(g, stmt, get_type_from_name(g, "void"));
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

bool is_integer_type(generator_t *g, type_t t) {
  (void)g;
  if (t.is_ptr) {
    return false;
  }
  if (t.name != NULL) {
    if (strcmp(t.name, "int") == 0)
      return true;
    if (strcmp(t.name, "i64") == 0)
      return true;
    if (strcmp(t.name, "i32") == 0)
      return true;
    if (strcmp(t.name, "i16") == 0)
      return true;
    if (strcmp(t.name, "i8") == 0)
      return true;
    if (strcmp(t.name, "u64") == 0)
      return true;
    if (strcmp(t.name, "u32") == 0)
      return true;
    if (strcmp(t.name, "u16") == 0)
      return true;
    if (strcmp(t.name, "u8") == 0)
      return true;
  }
  return LLVMGetTypeKind(t.type) == LLVMIntegerTypeKind;
}
