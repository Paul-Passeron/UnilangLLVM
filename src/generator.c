/**
 * generator.c
 * Copyright (C) 2024 Paul Passeron
 * GENERATOR source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/generator.h"
#include "../include/regexp.h"
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

generator_t *gen = NULL;

void generator_free(generator_t *g) {
  (void)g;
  printf("%s:%d TODO: free generator\n", __FILE__, __LINE__);
}

void set_global_generator(generator_t *g) { gen = g; }

generator_t *get_global_generator() { return gen; }

type_t get_ptr_of(type_t t) {
  type_t *ptr = malloc(sizeof(type_t));
  *ptr = t;
  type_t res = {0};
  res.type = LLVMPointerType(t.type, 0);
  res.name = NULL;
  res.kind = PTR;
  res.as.pointed_by = ptr;
  return res;
}

type_t get_type_from_name(const char *name) {
  for (size_t i = 0; i < gen->types.count; i++) {
    type_t t = gen->types.items[i];
    if (t.name == NULL) {
      continue;
    }
    if (strcmp(t.name, name) == 0) {
      return t;
    }
  }
  return (type_t){0};
}

type_t get_type_from_llvm(LLVMTypeRef type) {
  for (size_t i = 0; i < gen->types.count; i++) {
    type_t t = gen->types.items[i];
    if (t.type == type) {
      return t;
    }
  }
  return (type_t){0};
}

void add_type(type_t t) { da_append(&gen->types, t); }

void add_class(class_entry_t c) { da_append(&gen->classes, c); }

void add_function(function_entry_t f) { da_append(&gen->functions, f); }

void add_named_value(named_value_entry_t n) {
  da_append(&gen->named_values, n);
}

void add_builtin_types() {
  type_t int_t = {"int", BUILTIN, LLVMInt32TypeInContext(gen->context), {0}};
  type_t char_t = {"char", BUILTIN, LLVMInt8TypeInContext(gen->context), {0}};
  type_t i64_t = {"i64", BUILTIN, LLVMInt64TypeInContext(gen->context), {0}};
  type_t i32_t = {"i32", BUILTIN, LLVMInt32TypeInContext(gen->context), {0}};
  type_t i16_t = {"i16", BUILTIN, LLVMInt16TypeInContext(gen->context), {0}};
  type_t i8_t = {"i8", BUILTIN, LLVMInt8TypeInContext(gen->context), {0}};
  type_t u64_t = {"u64", BUILTIN, LLVMInt64TypeInContext(gen->context), {0}};
  type_t u32_t = {"u32", BUILTIN, LLVMInt32TypeInContext(gen->context), {0}};
  type_t u16_t = {"u16", BUILTIN, LLVMInt16TypeInContext(gen->context), {0}};
  type_t u8_t = {"u8", BUILTIN, LLVMInt8TypeInContext(gen->context), {0}};
  type_t void_t = {"void", BUILTIN, LLVMVoidTypeInContext(gen->context), {0}};
  type_t bool_t = {"bool", BUILTIN, LLVMInt1TypeInContext(gen->context), {0}};

  add_type(int_t);
  add_type(char_t);
  add_type(i64_t);
  add_type(i32_t);
  add_type(i16_t);
  add_type(i8_t);
  add_type(u64_t);
  add_type(u32_t);
  add_type(u16_t);
  add_type(u8_t);
  add_type(void_t);
  add_type(bool_t);
}

function_entry_t get_putstring() {
  strings ss = {0};
  types ts = {0};
  da_append(&ss, "s");
  da_append(&ts, get_ptr_of(get_type_from_name("char")));
  type_t void_t = get_type_from_name("void");
  function_entry_t putstring = {"put_string", ss, ts, void_t, 0};
  return putstring;
}

function_entry_t get_putchar() {
  strings ss = {0};
  types ts = {0};
  da_append(&ss, "c");
  da_append(&ts, get_type_from_name("char"));
  type_t void_t = get_type_from_name("void");
  function_entry_t res = {"put_character", ss, ts, void_t, 0};
  return res;
}

function_entry_t get_syscall() {
  strings ss = {0};
  types ts = {0};
  type_t int_t = get_type_from_name("int");
  da_append(&ss, "sysno");
  da_append(&ts, int_t);
  function_entry_t res = {"syscall", ss, ts, int_t, 1};
  return res;
}

void add_builtin_functions() {
  function_entry_t ps = get_putstring();
  function_entry_t pc = get_putchar();
  function_entry_t sc = get_syscall();
  add_function(ps);
  add_function(pc);
  add_function(sc);
  for (size_t i = 0; i < gen->functions.count; ++i) {
    function_entry_t f = gen->functions.items[i];
    LLVMAddFunction(gen->module, f.name, ftype_from_entry(f));
  }
}

void generator_init(generator_t *g) {

  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
  LLVMInitializeNativeAsmParser();

  g->context = LLVMContextCreate();
  g->builder = LLVMCreateBuilderInContext(g->context);
  g->module = LLVMModuleCreateWithNameInContext("main", g->context);

  g->functions = (functions){0};
  g->named_values = (named_values){0};
  g->classes = (classes){0};
  g->types = (types){0};
  g->current_function = NULL;

  set_global_generator(g);

  add_builtin_types();
  add_builtin_functions();
}

int get_named_values_scope() { return gen->named_values.count; }

void reset_named_values_to_scope(int scope) {
  for (int i = gen->named_values.count - 1; i >= scope; i--) {
    free(gen->named_values.items[i].name);
  }
  gen->named_values.count = scope;
}

void overwrite_pushed_value(LLVMValueRef value, char *name, type_t t) {
  bool worked = false;
  named_value_entry_t entry = {name, t, value};
  for (int i = gen->named_values.count - 1; i >= 0; --i) {
    if (strcmp(gen->named_values.items[i].name, name) == 0) {
      gen->named_values.items[i].value = value;
      worked = true;
      break;
    }
  }
  if (!worked) {
    add_named_value(entry);
  }
}

type_t get_type_from_ast(ast_t *type) {
  if (type->as.type.ptr_n == 0) {
    string_view_t tok = type->as.type.name.lexeme;
    char *name = strndup(tok.contents, tok.length);
    type_t res = get_type_from_name(name);
    free(name);
    return res;
  }
  ast_t new = *type;
  new.as.type.ptr_n = 0;
  type_t original = get_type_from_ast(&new);
  for (size_t i = 0; i < type->as.type.ptr_n; ++i) {
    original = get_ptr_of(original);
  }
  return original;
}

void generate_decl(ast_t *decl) {
  switch (decl->kind) {
  case AST_FUNDEF: {
    generate_fundef(decl);
  } break;
  case AST_VARDEF: {
    generate_vardef(decl);
  } break;
  case AST_CLASS: {
    generate_classdef(decl);
  } break;
  case AST_CT_CTE: {
    generate_ct_cte(decl);
  } break;
  default: {
    printf("Unexpected declaration kind: ");
    dump_ast(decl);
    printf("\n");
    exit(1);
  }
  }
}

void generate_statement(ast_t *stmt) {
  switch (stmt->kind) {
  case AST_VARDEF: {
    generate_vardef(stmt);
  } break;
  case AST_IFSTMT: {
    generate_ifstmt(stmt);
  } break;
  case AST_WHILE: {
    generate_whilestmt(stmt);
  } break;
  case AST_COMPOUND: {
    generate_compound(stmt);
  } break;
  case AST_ASSIGN: {
    generate_assign(stmt);
  } break;
  case AST_RETURN: {
    generate_return(stmt);
  } break;
  default:
    generate_expression(stmt);
  }
}

void generate_program(ast_t *program) {
  ast_program_t prog = program->as.program;
  for (size_t i = 0; i < prog.elem_count; ++i) {
    ast_t *decl = prog.elems[i];
    generate_decl(decl);
  }
}

function_entry_t entry_from_fundef(ast_t *fundef) {
  strings arg_names = {0};
  types arg_types = {0};
  ast_fundef_t f = fundef->as.fundef;
  for (size_t i = 0; i < f.param_count; ++i) {
    char *arg_name = sv_to_cstr(f.param_names[i].lexeme);
    type_t arg_type = get_type_from_ast(f.param_types[i]);
    da_append(&arg_names, arg_name);
    da_append(&arg_types, arg_type);
  }
  char *name = sv_to_cstr(f.name.lexeme);
  type_t ret = get_type_from_ast(f.return_type);
  function_entry_t entry = {.name = name,
                            .arg_names = arg_names,
                            .arg_types = arg_types,
                            .return_type = ret,
                            0};
  return entry;
}

LLVMValueRef fptr_from_entry(function_entry_t f) {
  // No support for methods for the moment.
  return LLVMGetNamedFunction(gen->module, f.name);
}

LLVMTypeRef type_to_llvm(type_t t) { return t.type; }

LLVMTypeRef ftype_from_entry(function_entry_t f) {
  ltypes args = {0};
  for (size_t i = 0; i < f.arg_types.count; ++i) {
    LLVMTypeRef t = type_to_llvm(f.arg_types.items[i]);
    da_append(&args, t);
  }
  LLVMTypeRef ret = type_to_llvm(f.return_type);
  LLVMTypeRef ftype =
      LLVMFunctionType(ret, args.items, args.count, f.is_var_args);
  da_free(args);
  return ftype;
}

void push_params(function_entry_t entry, LLVMValueRef fptr) {
  for (size_t i = 0; i < entry.arg_names.count; ++i) {
    LLVMValueRef arg = LLVMGetParam(fptr, i);
    char *name = entry.arg_names.items[i];
    LLVMSetValueName(arg, name);
    type_t t = entry.arg_types.items[i];
    LLVMValueRef ptr = LLVMBuildAlloca(gen->builder, t.type, "ptr");
    LLVMBuildStore(gen->builder, arg, ptr);
    named_value_entry_t nv = {name, t, ptr};
    da_append(&gen->named_values, nv);
  }
}

void generate_compound(ast_t *compound) {
  int scope = get_named_values_scope();
  ast_compound_t body = compound->as.compound;
  for (size_t i = 0; i < body.elem_count; ++i) {
    generate_statement(body.elems[i]);
  }
  reset_named_values_to_scope(scope);
}

void generate_function_body(function_entry_t entry, ast_t *body) {
  if (gen->current_function == NULL) {
    gen->current_function = malloc(sizeof(function_entry_t));
  }
  *gen->current_function = entry;
  LLVMTypeRef ftype = ftype_from_entry(entry);

  LLVMValueRef fptr = LLVMAddFunction(gen->module, entry.name, ftype);

  LLVMBasicBlockRef bb_entry =
      LLVMAppendBasicBlockInContext(gen->context, fptr, "entry");

  LLVMPositionBuilderAtEnd(gen->builder, bb_entry);

  int scope = get_named_values_scope();

  push_params(entry, fptr);

  generate_compound(body);

  reset_named_values_to_scope(scope);

  if (strcmp(entry.return_type.name, "void") == 0) {
    if (LLVMGetBasicBlockTerminator(LLVMGetLastBasicBlock(fptr)) == NULL) {
      LLVMBuildRetVoid(gen->builder);
    }
  }
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

LLVMValueRef generate_intlit(token_t tok) {
  return LLVMConstInt(get_type_from_name("i32").type, token_to_int(tok), 1);
}

LLVMValueRef generate_stringlit(token_t tok) {
  string_view_t actual;
  actual.contents = tok.lexeme.contents + 1;
  actual.length = tok.lexeme.length - 2;
  char *cstr = strndup(actual.contents, actual.length);
  char *cstr2 = unescape_string(cstr);
  printf("Printing string %s\n", cstr2);
  LLVMValueRef str = LLVMBuildGlobalStringPtr(gen->builder, cstr2, "");
  free(cstr);
  free(cstr2);
  return str;
}

void generate_fundef(ast_t *fundef) {
  function_entry_t entry = entry_from_fundef(fundef);
  add_function(entry);
  generate_function_body(entry, fundef->as.fundef.body);
}

function_entry_t f_by_name(const char *name) {
  for (size_t i = 0; i < gen->functions.count; i++) {
    if (strcmp(gen->functions.items[i].name, name) == 0) {
      return gen->functions.items[i];
    }
  }
  printf("%s:%d : error: undefined function '%s'\n", __FILE__, __LINE__, name);
  exit(1);
}

function_entry_t get_global_function(ast_t *called) {
  string_view_t lexeme = called->as.identifier.tok.lexeme;
  char *name = sv_to_cstr(lexeme);
  function_entry_t entry = f_by_name(name);
  free(name);
  return entry;
}

method_t get_method_by_name(class_entry_t cdef, const char *name) {
  for (size_t i = 0; i < cdef.methods.count; i++) {
    if (strcmp(cdef.methods.items[i].name, name) == 0) {
      return cdef.methods.items[i];
    }
  }
  printf("error: undefined method '%s' in class %s.\n", name, cdef.name);
  exit(1);
}

type_t get_return_type(ast_t *funcall) {
  if (funcall->as.funcall.called->kind == AST_IDENTIFIER) {
    function_entry_t entry = get_global_function(funcall->as.funcall.called);
    return entry.return_type;
  }
  ast_t *called = funcall->as.funcall.called;
  if (called->kind == AST_BINOP) {
    if (called->as.binop.op.kind != ACCESS) {
      printf("%s:%d : error: cannot call binary expression ", __FILE__,
             __LINE__);
      dump_ast(called);
      printf("\n");
      exit(1);
    }
    type_t t = t_of_expr(called->as.binop.lhs);
    if (t.kind == PTR) {
      t = dereference_type(t);
    }
    if (t.kind != CLASS) {
      printf("error: cannot call non-class method\n");
      exit(1);
    }
    class_entry_t cdef = *t.as.clazz;
    if (called->as.binop.rhs->kind != AST_IDENTIFIER) {
      printf("error: cannot call method with non-identifier name\n");
      exit(1);
    }
    char *name = sv_to_cstr(called->as.binop.rhs->as.identifier.tok.lexeme);
    method_t m = get_method_by_name(cdef, name);
    return m.return_type;
  }
  printf("Unreachable\n");
  exit(1);
}

LLVMValueRef generate_funcall(ast_t *funcall) {
  if (funcall->as.funcall.called->kind == AST_IDENTIFIER) {
    function_entry_t entry = get_global_function(funcall->as.funcall.called);
    LLVMValueRef called = LLVMGetNamedFunction(gen->module, entry.name);
    lvalues args = {0};
    if (funcall->as.funcall.arg_count != entry.arg_names.count &&
        !entry.is_var_args) {
      printf("error: function %s takes %ld arguments but %ld were given.\n",
             entry.name, entry.arg_names.count, funcall->as.funcall.arg_count);
      exit(1);
    }
    for (size_t i = 0; i < funcall->as.funcall.arg_count; ++i) {
      type_t t = entry.arg_types.items[i];
      ast_t *arg = funcall->as.funcall.args[i];
      LLVMValueRef arg_val = generate_expression(arg);
      if (i < entry.arg_names.count) {
        if (LLVMTypeOf(arg_val) != t.type) {
          arg_val = generate_cast(arg_val, t);
        }
      }
      da_append(&args, arg_val);
    }
    LLVMValueRef result = LLVMBuildCall2(gen->builder, ftype_from_entry(entry),
                                         called, args.items, args.count, "");
    da_free(args);
    return result;
  }
  ast_t *called = funcall->as.funcall.called;
  if (called->kind == AST_BINOP) {
    if (called->as.binop.op.kind != ACCESS) {
      printf("%s:%d : error: cannot call binary expression ", __FILE__,
             __LINE__);
      dump_ast(called);
      printf("\n");
      exit(1);
    }
    type_t t = t_of_expr(called->as.binop.lhs);
    if (t.kind == PTR) {
      t = dereference_type(t);
    }
    if (t.kind != CLASS) {
      printf("error: cannot call non-class method\n");
      exit(1);
    }

    class_entry_t cdef = *t.as.clazz;
    if (called->as.binop.rhs->kind != AST_IDENTIFIER) {
      printf("error: cannot call method with non-identifier name\n");
      exit(1);
    }
    char *name = sv_to_cstr(called->as.binop.rhs->as.identifier.tok.lexeme);
    method_t m = get_method_by_name(cdef, name);
    LLVMValueRef left = get_lm_pointer(called->as.binop.lhs);
    lvalues args = {0};
    da_append(&args, left);
    if (funcall->as.funcall.arg_count != m.arg_names.count) {
      printf("error: expected %ld arguments for method %s of class %s but got "
             "%ld.\n",
             m.arg_names.count, name, cdef.name, funcall->as.funcall.arg_count);
      exit(1);
    }
    for (size_t i = 0; i < m.arg_names.count; ++i) {
      ast_t *expr = funcall->as.funcall.args[i];
      type_t arg_t = t_of_expr(expr);
      LLVMValueRef arg = generate_expression(expr);
      if (LLVMTypeOf(arg) != arg_t.type) {
        arg = generate_cast(arg, arg_t);
      }
      da_append(&args, arg);
    }
    LLVMTypeRef ftype = ftype_from_method(m, cdef);
    LLVMValueRef fptr = fptr_from_method(m, cdef);
    LLVMValueRef res =
        LLVMBuildCall2(gen->builder, ftype, fptr, args.items, args.count, "");
    da_free(args);
    free(name);
    return res;
  }
  printf("Unreachable\n");
  exit(1);
}

int get_index_of_field(const char *field_name, class_entry_t cdef) {
  for (size_t i = 0; i < cdef.members.count; i++) {
    if (strcmp(field_name, cdef.members.items[i].name) == 0) {
      return i;
    }
  }
  printf("No field %s in class %s.\n", field_name, cdef.name);
  exit(1);
}

type_t t_of_expr(ast_t *expr) {
  switch (expr->kind) {
  case AST_INTLIT: {
    return get_type_from_name("i32");
  } break;
  case AST_STRINGLIT: {
    return get_ptr_of(get_type_from_name("char"));
  } break;
  case AST_BINOP: {
    if (expr->as.binop.op.kind == ACCESS) {
      type_t lhs = t_of_expr(expr->as.binop.lhs);
      if (lhs.kind == PTR) {
        lhs = dereference_type(lhs);
      }
      if (lhs.kind != CLASS) {
        printf("Cannot access field of non-class type %s\n", lhs.name);
        exit(1);
      }
      class_entry_t c = *lhs.as.clazz;
      if (expr->as.binop.rhs->kind != AST_IDENTIFIER) {
        printf("Expected identifier for field name\n");
        exit(1);
      }
      char *field_name =
          sv_to_cstr(expr->as.binop.rhs->as.identifier.tok.lexeme);
      int index = get_index_of_field(field_name, c);
      free(field_name);
      return c.members.items[index].type;
    }
    type_t lt = t_of_expr(expr->as.binop.lhs);
    type_t rt = t_of_expr(expr->as.binop.rhs);
    if (lt.type == rt.type) {
      return lt;
    }
    if (lt.kind == PTR) {
      return lt;
    }
    if (rt.kind == PTR) {
      return rt;
    }
    if (LLVMSizeOf(type_to_llvm(lt)) > LLVMSizeOf(type_to_llvm(rt))) {
      return lt;
    }
    return rt;
  } break;
  case AST_IDENTIFIER: {
    char *name = sv_to_cstr(expr->as.identifier.tok.lexeme);

    int index = get_named_value(name);
    if (index < 0) {
      printf("Identifier %s not declared in the current scope\n", name);
      exit(1);
    }
    type_t res = gen->named_values.items[index].t;
    free(name);
    return res;
  } break;
  case AST_BOOLLIT: {
    return get_type_from_name("bool");
  }
  case AST_FUNCALL: {
    return get_return_type(expr);
  }
  case AST_UNOP: {
    if (expr->as.unop.op.kind == DEREF) {
      type_t t = t_of_expr(expr->as.unop.operand);
      return dereference_type(t);
    }
    printf("%s:%d TODO: get type of unop\n", __FILE__, __LINE__);
    exit(1);
  } break;
  default: {
    printf("%s:%d TODO: get type of expression %d\n", __FILE__, __LINE__,
           expr->kind);
    exit(1);
  }
  }
}

LLVMValueRef generate_binop(ast_t *binop) {
  LLVMValueRef lhs = generate_expression(binop->as.binop.lhs);
  LLVMValueRef rhs = generate_expression(binop->as.binop.rhs);
  type_t lt = t_of_expr(binop->as.binop.lhs);
  type_t rt = t_of_expr(binop->as.binop.rhs);
  bool is_ptr = false;
  if (lt.type != rt.type) {
    if (lt.kind == PTR) {
      is_ptr = true;
      lhs = generate_cast(lhs, rt);
    } else if (rt.kind == PTR) {
      is_ptr = true;
      rhs = generate_cast(rhs, lt);
    } else if (LLVMSizeOf(type_to_llvm(lt)) > LLVMSizeOf(type_to_llvm(rt))) {
      rhs = generate_cast(rhs, lt);
    } else {
      lhs = generate_cast(lhs, rt);
    }
  }
  LLVMValueRef res;
  switch (binop->as.binop.op.kind) {
  case PLUS: {
    res = LLVMBuildAdd(gen->builder, lhs, rhs, "");
  } break;
  case MINUS: {
    res = LLVMBuildSub(gen->builder, lhs, rhs, "");
  } break;
  case MULT: {
    res = LLVMBuildMul(gen->builder, lhs, rhs, "");
  } break;
  case DIV: {
    res = LLVMBuildSDiv(gen->builder, lhs, rhs, "");
  } break;
  case MODULO: {
    res = LLVMBuildSRem(gen->builder, lhs, rhs, "");
  } break;
  default: {
    printf("%s:%d TODO: generate binary operation %d\n", __FILE__, __LINE__,
           binop->as.binop.op.kind);
    exit(1);
  }
  }
  if (is_ptr) {
    res = generate_cast(res, get_ptr_of(get_type_from_name("void")));
  }
  return res;
}

LLVMValueRef generate_access(ast_t *binop) {
  type_t t = t_of_expr(binop);
  LLVMValueRef ptr = get_lm_pointer(binop);
  LLVMValueRef res = LLVMBuildLoad2(gen->builder, t.type, ptr, "");
  return res;
}

bool is_type_int(type_t t) {
  return t.kind == BUILTIN && strcmp(t.name, "void") != 0;
}

int get_named_value(char *name) {
  for (int i = gen->named_values.count - 1; i >= 0; --i) {
    if (strcmp(gen->named_values.items[i].name, name) == 0) {
      return i;
    }
  }
  return -1;
}

LLVMValueRef generate_deref(ast_t *expr) {
  type_t t = t_of_expr(expr->as.unop.operand);
  if (t.kind != PTR) {
    printf("Cannot dereference non-pointer type ");
    fflush(stdout);
    LLVMDumpType(t.type);
    fflush(stdout);
    printf(".\n");
    fflush(stdout);
    exit(1);
  }
  LLVMValueRef ptr = generate_expression(expr->as.unop.operand);
  type_t res_type = dereference_type(t);
  LLVMValueRef res = LLVMBuildLoad2(gen->builder, res_type.type, ptr, "");
  return res;
}

LLVMValueRef generate_unop(ast_t *expr) {
  if (expr->as.unop.op.kind == DEREF) {
    return generate_deref(expr);
  }
  printf("%s:%d TODO: generate_unop.\n", __FILE__, __LINE__);
  exit(1);
}

LLVMValueRef generate_expression(ast_t *expr) {
  switch (expr->kind) {
  case AST_INTLIT: {
    return generate_intlit(expr->as.intlit.tok);
  } break;
  case AST_STRINGLIT: {
    return generate_stringlit(expr->as.stringlit.tok);
  } break;
  case AST_FUNCALL: {
    return generate_funcall(expr);
  } break;
  case AST_BINOP: {
    if (expr->as.binop.op.kind == ACCESS) {
      return generate_access(expr);
    }
    return generate_binop(expr);
  } break;
  case AST_IDENTIFIER: {
    char *name = sv_to_cstr(expr->as.identifier.tok.lexeme);
    int index = get_named_value(name);
    if (index < 0) {
      printf("Identifier %s not declared in this scope\n", name);
    }
    named_value_entry_t entry = gen->named_values.items[index];
    LLVMValueRef ptr = entry.value;
    type_t t = entry.t;
    LLVMValueRef res = LLVMBuildLoad2(gen->builder, t.type, ptr, name);
    free(name);
    return res;
  }
  case AST_BOOLLIT: {
    int val = expr->as.boollit.val;
    return LLVMConstInt(get_type_from_name("bool").type, val, 0);
  }
  case AST_UNOP: {
    return generate_unop(expr);
  }
  default:
    printf("%s:%d TODO: generate_expression %d\n", __FILE__, __LINE__,
           expr->kind);
    exit(1);
  }
}

void generate_ifstmt(ast_t *if_stmt) {
  type_t t = t_of_expr(if_stmt->as.if_stmt.cond);
  type_t bool_t = get_type_from_name("bool");
  LLVMValueRef cond = generate_expression(if_stmt->as.if_stmt.cond);
  if (t.type != bool_t.type) {
    cond = generate_cast(cond, bool_t);
  }
  function_entry_t entry = *gen->current_function;
  LLVMValueRef fptr = fptr_from_entry(entry);
  LLVMBasicBlockRef then_block = LLVMAppendBasicBlock(fptr, "then");
  LLVMBasicBlockRef else_block = LLVMAppendBasicBlock(fptr, "else");
  LLVMBasicBlockRef merge_block = LLVMAppendBasicBlock(fptr, "merge");
  LLVMBuildCondBr(gen->builder, cond, then_block, else_block);
  LLVMPositionBuilderAtEnd(gen->builder, then_block);
  generate_statement(if_stmt->as.if_stmt.body);
  LLVMBuildBr(gen->builder, merge_block);
  LLVMPositionBuilderAtEnd(gen->builder, else_block);
  if (if_stmt->as.if_stmt.other_body) {
    generate_statement(if_stmt->as.if_stmt.other_body);
  }
  LLVMBuildBr(gen->builder, merge_block);
  LLVMPositionBuilderAtEnd(gen->builder, merge_block);
}
void generate_whilestmt(ast_t *while_stmt) {
  (void)while_stmt;
  ast_t *cond = while_stmt->as.while_stmt.cond;
  ast_t *body = while_stmt->as.while_stmt.body;
  LLVMValueRef fptr = fptr_from_entry(*gen->current_function);
  LLVMBasicBlockRef bb_cond = LLVMAppendBasicBlock(fptr, "cond");
  LLVMBasicBlockRef bb_body = LLVMAppendBasicBlock(fptr, "body");
  LLVMBasicBlockRef bb_after = LLVMAppendBasicBlock(fptr, "after");

  LLVMBuildBr(gen->builder, bb_cond);

  LLVMPositionBuilderAtEnd(gen->builder, bb_cond);
  LLVMValueRef cond_expr = generate_expression(cond);

  type_t bool_t = get_type_from_name("bool");
  if (LLVMTypeOf(cond_expr) != bool_t.type) {
    cond_expr = generate_cast(cond_expr, bool_t);
  }
  LLVMBuildCondBr(gen->builder, cond_expr, bb_body, bb_after);
  LLVMPositionBuilderAtEnd(gen->builder, bb_body);
  generate_statement(body);
  LLVMBuildBr(gen->builder, bb_cond);
  LLVMPositionBuilderAtEnd(gen->builder, bb_after);
}

type_t t_from_cdef(class_entry_t cdef) {
  type_t t;
  LLVMTypeRef str = LLVMStructCreateNamed(gen->context, cdef.name);
  ltypes mems = {0};
  for (size_t i = 0; i < cdef.members.count; ++i) {
    member_t m = cdef.members.items[i];
    da_append(&mems, m.type.type);
  }
  LLVMStructSetBody(str, mems.items, mems.count, 0);
  da_free(mems);
  t.kind = CLASS;
  t.as.clazz = malloc(sizeof(class_entry_t));
  *t.as.clazz = cdef;
  t.type = str;
  t.name = cdef.name;
  return t;
}

LLVMTypeRef ftype_from_constructor(constructor_t constructor,
                                   class_entry_t cdef) {
  ltypes ts = {0};
  type_t self = get_type_from_name(cdef.name);
  da_append(&ts, get_ptr_of(self).type);
  for (size_t j = 0; j < constructor.arg_names.count; j++) {
    type_t t = constructor.arg_types.items[j];
    da_append(&ts, t.type);
  }
  LLVMTypeRef ftype =
      LLVMFunctionType(get_type_from_name("void").type, ts.items, ts.count, 0);
  da_free(ts);
  return ftype;
}

LLVMValueRef fptr_from_constructor(constructor_t constructor,
                                   class_entry_t cdef, size_t i) {
  (void)constructor;
  char name[1024] = {0};
  sprintf(name, "%s_%ld", cdef.name, i);
  LLVMValueRef fptr = LLVMGetNamedFunction(gen->module, name);
  return fptr;
}

void declare_constructors(class_entry_t cdef) {
  for (size_t i = 0; i < cdef.constructors.count; i++) {
    constructor_t constructor = cdef.constructors.items[i];
    char name[1024] = {0};
    sprintf(name, "%s_%ld", cdef.name, i);
    LLVMTypeRef ftype = ftype_from_constructor(constructor, cdef);
    LLVMAddFunction(gen->module, name, ftype);
  }
}

LLVMTypeRef ftype_from_method(method_t method, class_entry_t cdef) {
  ltypes ts = {0};
  type_t self = get_type_from_name(cdef.name);
  da_append(&ts, get_ptr_of(self).type);
  for (size_t j = 0; j < method.arg_names.count; j++) {
    type_t t = method.arg_types.items[j];
    da_append(&ts, t.type);
  }
  LLVMTypeRef ftype =
      LLVMFunctionType(method.return_type.type, ts.items, ts.count, 0);
  da_free(ts);
  return ftype;
}

LLVMValueRef fptr_from_method(method_t method, class_entry_t cdef) {
  char method_name[1024] = {0};
  sprintf(method_name, "%s_%s", cdef.name, method.name);
  LLVMValueRef fptr = LLVMGetNamedFunction(gen->module, method_name);
  return fptr;
}

void declare_methods(class_entry_t cdef) {
  for (size_t i = 0; i < cdef.methods.count; i++) {
    method_t method = cdef.methods.items[i];
    LLVMTypeRef ftype = ftype_from_method(method, cdef);
    char method_name[1024] = {0};
    sprintf(method_name, "%s_%s", cdef.name, method.name);
    LLVMAddFunction(gen->module, method_name, ftype);
  }
}

void push_method_params(method_t method, LLVMValueRef fptr) {
  for (size_t j = 0; j < method.arg_names.count; j++) {
    type_t t = method.arg_types.items[j];
    LLVMValueRef param = LLVMGetParam(fptr, j);
    char *name = strdup(method.arg_names.items[j]);
    LLVMSetValueName(param, name);
    LLVMValueRef ptr = LLVMBuildAlloca(gen->builder, t.type, "ptr");
    LLVMBuildStore(gen->builder, param, ptr);
    named_value_entry_t entry = {name, t, ptr};
    add_named_value(entry);
  }
}

void generate_method(method_t method, class_entry_t cdef, ast_t *m) {
  // LLVMTypeRef ftype = ftype_from_method(method, cdef);
  LLVMValueRef fptr = fptr_from_method(method, cdef);
  char name[256] = {0};
  sprintf(name, "%s_%s", cdef.name, method.name);
  strings names = {0};
  types types = {0};
  da_append(&names, strdup("self"));
  da_append(&types, get_type_from_name(cdef.name));
  for (size_t j = 0; j < method.arg_names.count; j++) {
    da_append(&types, method.arg_types.items[j]);
    da_append(&names, method.arg_names.items[j]);
  }
  function_entry_t func = {strdup(name), names, types, method.return_type, 0};
  if (gen->current_function == NULL) {
    gen->current_function = malloc(sizeof(function_entry_t));
  }
  *gen->current_function = func;
  LLVMBasicBlockRef bb_entry =
      LLVMAppendBasicBlockInContext(gen->context, fptr, "entry");
  LLVMPositionBuilderAtEnd(gen->builder, bb_entry);

  LLVMValueRef self = LLVMGetParam(fptr, 0);
  named_value_entry_t self_entry = {
      strdup("self"),
      get_ptr_of(get_type_from_name(cdef.name)),
      self,
  };
  int scope = get_named_values_scope();
  add_named_value(self_entry);
  push_method_params(method, fptr);
  generate_compound(m->as.method.fdef->as.fundef.body);
  reset_named_values_to_scope(scope);
  if (strcmp(method.return_type.name, "void") == 0) {
    if (LLVMGetBasicBlockTerminator(LLVMGetLastBasicBlock(fptr)) == NULL) {
      LLVMBuildRetVoid(gen->builder);
    }
  }
}

bool is_ast_constructor(ast_t *ast) {
  if (ast->kind != AST_METHOD) {
    return false;
  }
  ast_method_t m = ast->as.method;
  return m.fdef->as.fundef.return_type == NULL;
}

void push_constructor_params(constructor_t c, LLVMValueRef fptr) {
  for (size_t j = 0; j < c.arg_names.count; j++) {
    type_t t = c.arg_types.items[j];
    LLVMValueRef param = LLVMGetParam(fptr, j + 1);
    char *name = strdup(c.arg_names.items[j]);
    LLVMSetValueName(param, name);
    LLVMValueRef ptr = LLVMBuildAlloca(gen->builder, t.type, "ptr");
    LLVMBuildStore(gen->builder, param, ptr);
    named_value_entry_t entry = {name, t, ptr};
    add_named_value(entry);
  }
}

void generate_constructor(constructor_t c, class_entry_t cdef, int index,
                          ast_t *body) {
  // LLVMTypeRef ftype = ftype_from_constructor(c, cdef);
  LLVMValueRef fptr = fptr_from_constructor(c, cdef, index);

  char name[256] = {0};
  sprintf(name, "%s_%d", cdef.name, index);
  strings names = {0};
  types types = {0};
  da_append(&names, strdup("self"));
  da_append(&types, get_type_from_name(cdef.name));
  for (size_t j = 0; j < c.arg_names.count; j++) {
    da_append(&types, c.arg_types.items[j]);
    da_append(&names, c.arg_names.items[j]);
  }
  function_entry_t func = {strdup(name), names, types,
                           get_type_from_name("void"), 0};
  if (gen->current_function == NULL) {
    gen->current_function = malloc(sizeof(function_entry_t));
  }
  *gen->current_function = func;

  LLVMBasicBlockRef bb_entry =
      LLVMAppendBasicBlockInContext(gen->context, fptr, "entry");
  LLVMPositionBuilderAtEnd(gen->builder, bb_entry);
  LLVMValueRef self = LLVMGetParam(fptr, 0);
  LLVMSetValueName(self, "self");
  named_value_entry_t self_entry = {
      strdup("self"),
      get_ptr_of(get_type_from_name(cdef.name)),
      self,
  };
  int scope = get_named_values_scope();
  add_named_value(self_entry);
  push_constructor_params(c, fptr);
  generate_compound(body);
  reset_named_values_to_scope(scope);
  if (LLVMGetBasicBlockTerminator(LLVMGetLastBasicBlock(fptr)) == NULL) {
    LLVMBuildRetVoid(gen->builder);
  }
}

void generate_classdef(ast_t *classdef) {
  class_entry_t cdef = entry_from_cdef(classdef->as.clazz);
  type_t t = t_from_cdef(cdef);
  add_type(t);
  declare_constructors(cdef);
  declare_methods(cdef);
  int method_count = 0;
  int constructor_count = 0;

  for (size_t i = 0; i < classdef->as.clazz.field_count; ++i) {
    ast_t *field = classdef->as.clazz.fields[i];
    if (field->kind == AST_METHOD) {
      if (is_ast_constructor(field)) {
        int index = constructor_count++;
        constructor_t c = cdef.constructors.items[index];
        generate_constructor(c, cdef, index,
                             field->as.method.fdef->as.fundef.body);

      } else {
        method_t m = cdef.methods.items[method_count++];
        generate_method(m, cdef, field);
      }
    }
  }
}

type_t dereference_type(type_t t) {
  if (t.kind != PTR) {
    printf("Cannot dereference non-pointer type\n");
    exit(1);
  }
  return *t.as.pointed_by;
}

LLVMValueRef get_lm_pointer(ast_t *lm) {
  if (lm->kind == AST_IDENTIFIER) {
    char *name = sv_to_cstr(lm->as.identifier.tok.lexeme);
    int index = get_named_value(name);
    if (index < 0) {
      printf("Identifier '%s' not declared in the current scope\n", name);
      exit(1);
    }
    free(name);
    return gen->named_values.items[index].value;
  }
  if (lm->kind == AST_BINOP) {
    if (lm->as.binop.op.kind == ACCESS) {
      type_t base_type = t_of_expr(lm->as.binop.lhs);
      LLVMValueRef base = get_lm_pointer(lm->as.binop.lhs);
      if (base_type.kind == PTR) {
        base_type = dereference_type(base_type);
        // base = LLVMBuildLoad2(gen->builder, base_type.type, base, "");
      }

      if (base_type.kind != CLASS) {
        printf("Cannot get access lm pointer from non-class type\n");
        exit(1);
      }
      class_entry_t cdef = *base_type.as.clazz;
      if (lm->as.binop.rhs->kind != AST_IDENTIFIER) {
        printf("Malformed access expression: ");
        dump_ast(lm);
        printf("\n");
        exit(1);
      }
      char *field_name = sv_to_cstr(lm->as.binop.rhs->as.identifier.tok.lexeme);
      int index = get_index_of_field(field_name, cdef);
      LLVMValueRef res =
          LLVMBuildStructGEP2(gen->builder, base_type.type, base, index, "");
      free(field_name);
      return res;
    } else {
      printf("%s:%d TODO: binop in lm\n", __FILE__, __LINE__);
      exit(1);
    }
  }
  printf("%s:%d TODO: get_lm_pointer\n", __FILE__, __LINE__);
  exit(1);
}

void update_value(ast_t *lhs, LLVMValueRef rhs) {
  LLVMValueRef ptr = get_lm_pointer(lhs);
  type_t target_type = t_of_expr(lhs);
  if (target_type.type != LLVMTypeOf(rhs)) {
    rhs = generate_cast(rhs, target_type);
  }
  LLVMBuildStore(gen->builder, rhs, ptr);
}

void generate_assign(ast_t *assign) {
  ast_t *lm = assign->as.assign.lhs;
  ast_t *rm = assign->as.assign.rhs;
  LLVMValueRef rhs = generate_expression(rm);
  update_value(lm, rhs);
}

bool is_integer_type(type_t t) {
  if (t.kind != BUILTIN)
    return false;
  return strcmp(t.name, "void") != 0;
}

int get_constructor_with_single_arg_matching_type(class_entry_t cdef,
                                                  LLVMTypeRef t) {
  for (size_t i = 0; i < cdef.constructors.count; i++) {
    constructor_t c = cdef.constructors.items[i];
    if (c.arg_names.count != 1)
      continue;
    LLVMTypeRef ct = c.arg_types.items[0].type;
    if (ct == t) {
      return i;
    }
  }
  return -1;
}

LLVMValueRef generate_cast(LLVMValueRef value, type_t target_type) {
  LLVMTypeRef llvm_target_type = type_to_llvm(target_type);
  if (LLVMTypeOf(value) == llvm_target_type) {
    return value;
  }
  if (target_type.type == get_type_from_name("bool").type) {
    return LLVMBuildICmp(gen->builder, LLVMIntNE, value,
                         LLVMConstInt(LLVMTypeOf(value), 0, 0), "");
  }
  if (LLVMGetTypeKind(LLVMTypeOf(value)) == LLVMPointerTypeKind) {
    value = LLVMBuildPtrToInt(gen->builder, value,
                              get_type_from_name("int").type, "");
  }
  if (target_type.kind == PTR) {
    value = LLVMBuildIntToPtr(gen->builder, value, llvm_target_type, "");
  } else if (is_integer_type(target_type)) {
    value = LLVMBuildIntCast2(gen->builder, value, llvm_target_type, 1, "");
  } else {
    if (target_type.kind == CLASS) {
      // Look for a single argument constructor that matches values's type.
      // If found, call it.
      class_entry_t cdef = *target_type.as.clazz;
      int constructor_index = get_constructor_with_single_arg_matching_type(
          cdef, LLVMTypeOf(value));
      if (constructor_index < 0) {
        printf("Cannot convert type ");
        fflush(stdout);
        LLVMDumpType(LLVMTypeOf(value));
        fflush(stdout);
        printf(" to type ");
        fflush(stdout);
        LLVMDumpType(target_type.type);
        fflush(stdout);
        printf(".\n");
        exit(1);
      }
      constructor_t c = cdef.constructors.items[constructor_index];
      LLVMValueRef ptr = LLVMBuildAlloca(gen->builder, target_type.type, "");
      lvalues args = {0};
      da_append(&args, ptr);
      da_append(&args, value);
      LLVMTypeRef ftype = ftype_from_constructor(c, cdef);
      LLVMValueRef fptr = fptr_from_constructor(c, cdef, constructor_index);
      LLVMBuildCall2(gen->builder, ftype, fptr, args.items, args.count, "");
      da_free(args);
      LLVMValueRef res =
          LLVMBuildLoad2(gen->builder, target_type.type, ptr, "");
      return res;
    }
  }
  return value;
}

void generate_return(ast_t *ret) {
  if (gen->current_function == NULL) {
    printf("Error: Return statement outside of a function.\n");
    exit(1);
  }
  type_t target_type = gen->current_function->return_type;
  LLVMTypeRef llvm_target_type = type_to_llvm(target_type);
  if (ret->as.return_stmt.expr == NULL) {
    if (llvm_target_type != get_type_from_name("void").type) {
      printf("Cannot return void from a function that returns ");
      fflush(stdout);
      LLVMDumpType(llvm_target_type);
      fflush(stdout);
      printf(".\n");
      exit(1);
    }
    LLVMBuildRetVoid(gen->builder);
    return;
  }
  LLVMValueRef value = generate_expression(ret->as.return_stmt.expr);
  if (LLVMTypeOf(value) != llvm_target_type) {
    value = generate_cast(value, target_type);
  }
  LLVMBuildRet(gen->builder, value);
}

int get_default_constructor(class_entry_t c) {
  for (size_t i = 0; i < c.constructors.count; i++) {
    if (c.constructors.items[i].arg_names.count == 0)
      return i;
  }
  return -1;
}

LLVMValueRef generate_default_value_for_type(type_t t) {
  if (t.kind == BUILTIN) {
    return LLVMConstInt(t.type, 0, 1);
  } else if (t.kind == PTR) {
    return LLVMConstNull(t.type);
  }
  class_entry_t c = *t.as.clazz;
  int default_constructor_index = get_default_constructor(c);
  if (default_constructor_index < 0) {
    printf("Error: No default constructor found for class %s.\n", c.name);
    exit(1);
  }
  constructor_t cons = c.constructors.items[default_constructor_index];
  LLVMValueRef ptr = LLVMBuildAlloca(gen->builder, t.type, "");
  LLVMValueRef fptr = fptr_from_constructor(cons, c, default_constructor_index);
  LLVMTypeRef ftype = ftype_from_constructor(cons, c);
  LLVMBuildCall2(gen->builder, ftype, fptr, &ptr, 1, "");
  return LLVMBuildLoad2(gen->builder, t.type, ptr, "");
}

void generate_vardef(ast_t *vardef) {
  type_t type = get_type_from_ast(vardef->as.vardef.type);
  LLVMTypeRef llvm_type = type_to_llvm(type);
  LLVMValueRef expr;
  if (vardef->as.vardef.value != NULL) {
    expr = generate_expression(vardef->as.vardef.value);
  } else {
    expr = generate_default_value_for_type(type);
  }
  if (LLVMTypeOf(expr) != llvm_type) {
    expr = generate_cast(expr, type);
  }
  char *name = sv_to_cstr(vardef->as.vardef.name.lexeme);
  LLVMValueRef ptr = LLVMBuildAlloca(gen->builder, llvm_type, "var");
  LLVMBuildStore(gen->builder, expr, ptr);
  LLVMSetValueName(expr, name);
  named_value_entry_t entry = {
      name,
      type,
      ptr,
  };
  add_named_value(entry);
}

void generate_ct_cte(ast_t *ct_cte) {
  (void)ct_cte;
  printf("%s:%d TODO: generate_ct_cte\n", __FILE__, __LINE__);
  exit(1);
}

class_entry_t entry_from_cdef(ast_class_t cdef) {

  methods methods = {0};
  constructors constructors = {0};
  members members = {0};

  char *name = sv_to_cstr(cdef.name.lexeme);

  for (size_t i = 0; i < cdef.field_count; i++) {
    ast_t *field = cdef.fields[i];
    if (field->kind == AST_METHOD) {
      if (sv_eq(cdef.name.lexeme,
                field->as.method.fdef->as.fundef.name.lexeme)) {
        ast_method_t m = field->as.method;
        specifier_t spec =
            sv_eq(m.specifier.lexeme, SV("public")) ? PUBLIC : PRIVATE;
        strings arg_names = {0};
        types arg_types = {0};
        ast_fundef_t fundef = m.fdef->as.fundef;
        for (size_t j = 0; j < fundef.param_count; j++) {
          type_t type = get_type_from_ast(fundef.param_types[j]);
          char *name = sv_to_cstr(fundef.param_names[j].lexeme);
          da_append(&arg_names, name);
          da_append(&arg_types, type);
        }
        constructor_t entry = {spec, arg_names, arg_types};
        da_append(&constructors, entry);
      } else {
        ast_method_t m = field->as.method;
        char *method_name =
            sv_to_cstr(field->as.method.fdef->as.fundef.name.lexeme);
        specifier_t spec =
            sv_eq(m.specifier.lexeme, SV("public")) ? PUBLIC : PRIVATE;
        strings arg_names = {0};
        types arg_types = {0};
        ast_fundef_t fundef = m.fdef->as.fundef;
        for (size_t j = 0; j < fundef.param_count; j++) {
          type_t type = get_type_from_ast(fundef.param_types[j]);
          char *name = sv_to_cstr(fundef.param_names[j].lexeme);
          da_append(&arg_names, name);
          da_append(&arg_types, type);
        }
        type_t return_type = get_type_from_ast(fundef.return_type);
        method_t entry = {method_name, spec, arg_names, arg_types, return_type};
        da_append(&methods, entry);
      }
    } else {
      ast_member_t m = field->as.member;
      specifier_t spec =
          sv_eq(m.specifier.lexeme, SV("public")) ? PUBLIC : PRIVATE;
      type_t type = get_type_from_ast(m.var->as.vardef.type);
      ast_t *init = m.var->as.vardef.value;
      char *name = sv_to_cstr(m.var->as.vardef.name.lexeme);
      member_t entry = {name, spec, type, init};
      da_append(&members, entry);
    }
  }
  class_entry_t entry = {
      name,
      methods,
      constructors,
      members,
  };
  return entry;
}