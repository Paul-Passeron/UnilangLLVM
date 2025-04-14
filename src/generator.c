/**
 * generator.c
 * Copyright (C) 2024 Paul Passeron
 * GENERATOR source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/generator.h"
#include "../include/regexp.h"
#include "../include/unilang_lexer.h"
#include "../include/unilang_parser.h"
#include <linux/limits.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Transforms/PassBuilder.h>
#include <llvm-c/Types.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CRASH

#ifdef CRASH
#define EXIT                                                                   \
  print_types();                                                               \
  fflush(stdout);                                                              \
  *(char *)0 = 0;                                                              \
  exit(1)
#else
#define EXIT exit(1)
#endif

#define TODO printf("%s:%d TODO %s\n", __FILE__, __LINE__, __func__)

#define t_of_expr(ast) sanitize_type(t_of_expr_unsafe(ast))

generator_t *gen = NULL;

void generator_free(generator_t *g) {
  (void)g;
  TODO;
}

void set_global_generator(generator_t *g) { gen = g; }

generator_t *get_global_generator() { return gen; }

type_t get_ptr_of(type_t t) {
  type_t *ptr = malloc(sizeof(type_t));
  *ptr = t;
  type_t res = {0};
  res.type = NULL;
  if (t.kind != TEMPLATED) {
    res.type = LLVMPointerType(type_to_llvm(t), 0);
  }
  res.name = NULL;
  res.kind = PTR;
  res.pointed_by = ptr;
  return res;
}

char *get_include_postfix(ast_t *expr) {
  if (expr->kind == AST_IDENTIFIER) {
    return sv_to_cstr(expr->as.identifier.tok.lexeme);
  } else if (expr->kind != AST_BINOP || expr->as.binop.op.kind != ACCESS) {
    printf("Bad include expression: ");
    dump_ast(expr);
    printf("\n");
    EXIT;
  }
  char *left = get_include_postfix(expr->as.binop.lhs);
  int ll = strlen(left);
  char *right = get_include_postfix(expr->as.binop.rhs);
  int lr = strlen(right);
  char *res = malloc(ll + lr + 2);
  memcpy(res, left, ll);
  res[ll] = '/';
  memcpy(res + ll + 1, right, lr);
  res[ll + lr + 1] = 0;
  free(left);
  free(right);
  return res;
}

type_t get_type_from_name(const char *name) {
  for (size_t i = 0; i < gen->types.count; i++) {
    int index = gen->types.count - i - 1;
    type_t t = gen->types.items[index];
    if (t.name == NULL) {
      if (strcmp(((class_entry_t *)(t.pointed_by))->name, name)) {
        continue;
      }
      printf("Found Incomplete type\n");
      return t;
    }
    if (strcmp(t.name, name) == 0) {
      return t;
    }
  }
  printf("Type %s does not exist in the current context\n", name);
  printf("Context: ");
  print_types();
  EXIT;
}

type_t get_type_from_llvm(LLVMTypeRef type) {
  for (size_t i = 0; i < gen->types.count; i++) {
    type_t t = gen->types.items[i];
    t = sanitize_type(t);
    if (type_to_llvm(t) == type) {
      return t;
    }
  }
  printf("Could not find match for type ");
  fflush(stdout);
  LLVMDumpType(type);
  printf("\n");
  EXIT;
}

void print_type(type_t type) {
  if (type.name != NULL) {
    printf("%s = ", type.name);
  }
  if (type.kind == BUILTIN) {
    printf("<builtin type>");
  } else if (type.kind == PTR) {
    print_type(*type.pointed_by);
    printf("*");
  } else if (type.kind == ALIAS) {
    printf("alias(");
    print_type(*type.pointed_by);
    printf(")");
  } else if (type.kind == CLASS) {
    printf("class");
  } else if (type.kind == TEMPLATED) {
    printf("templated");
  } else {
    printf("<unknown type %d>", type.kind);
  }
}

void add_type(type_t t) {
  if (t.kind == CLASS) {
    printf("Class name being added as a type is %s\n", t.name);
  }
  da_append(&gen->types, t);
}

void add_class(class_entry_t c) {
  printf("Adding class %s\n", c.name);
  da_append(&gen->classes, c);
}

void add_function(function_entry_t f) { da_append(&gen->functions, f); }

void add_named_value(named_value_entry_t n) {
  da_append(&gen->named_values, n);
}

void add_builtin_types() {
  type_t int_t = {"int", BUILTIN, LLVMInt32TypeInContext(gen->context),
                  NULL,  NULL,    NULL};
  type_t char_t = {"char", BUILTIN, LLVMInt8TypeInContext(gen->context),
                   NULL,   NULL,    NULL};
  type_t i64_t = {"i64", BUILTIN, LLVMInt64TypeInContext(gen->context),
                  NULL,  NULL,    NULL};
  type_t i32_t = {"i32", BUILTIN, LLVMInt32TypeInContext(gen->context),
                  NULL,  NULL,    NULL};
  type_t i16_t = {"i16", BUILTIN, LLVMInt16TypeInContext(gen->context),
                  NULL,  NULL,    NULL};
  type_t i8_t = {"i8", BUILTIN, LLVMInt8TypeInContext(gen->context),
                 NULL, NULL,    NULL};
  type_t u64_t = {"u64", BUILTIN, LLVMInt64TypeInContext(gen->context),
                  NULL,  NULL,    NULL};
  type_t u32_t = {"u32", BUILTIN, LLVMInt32TypeInContext(gen->context),
                  NULL,  NULL,    NULL};
  type_t u16_t = {"u16", BUILTIN, LLVMInt16TypeInContext(gen->context),
                  NULL,  NULL,    NULL};
  type_t u8_t = {"u8", BUILTIN, LLVMInt8TypeInContext(gen->context),
                 NULL, NULL,    NULL};
  type_t void_t = {"void", BUILTIN, LLVMVoidTypeInContext(gen->context),
                   NULL,   NULL,    NULL};
  type_t bool_t = {"bool", BUILTIN, LLVMInt1TypeInContext(gen->context),
                   NULL,   NULL,    NULL};
  type_t float_t = {"float", BUILTIN, LLVMFloatTypeInContext(gen->context),
                    NULL,    NULL,    NULL};

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
  add_type(float_t);
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

function_entry_t get_malloc() {
  strings ss = {0};
  types ts = {0};
  da_append(&ss, "size");
  type_t void_ptr_t = get_ptr_of(get_type_from_name("void"));
  da_append(&ts, get_ptr_of(void_ptr_t));
  function_entry_t res = {"malloc", ss, ts, void_ptr_t, 0};
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

function_entry_t get_free() {
  strings ss = {0};
  types ts = {0};
  type_t void_t = get_type_from_name("void");
  da_append(&ss, "ptr");
  da_append(&ts, get_ptr_of(void_t));
  function_entry_t res = {"free", ss, ts, void_t, 0};
  return res;
}

void add_builtin_functions() {
  function_entry_t sc = get_syscall();
  function_entry_t mc = get_malloc();
  function_entry_t fr = get_free();
  add_function(sc);
  add_function(mc);
  add_function(fr);
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
  // g->classes_templates = (classes){0};
  g->templates = (templates){0};
  g->types = (types){0};
  g->defers = (defers){0};
  g->current_function = NULL;
  g->current_ptr = NULL;
  g->current_function_scope = 0;
  g->is_new = 0;
  g->last_bb = NULL;
  g->interfaces = (interfaces){0};
  g->inst_classes = (inst_classes){0};
  g->included_files = (strings){0};
  set_global_generator(g);
  add_builtin_types();
  add_builtin_functions();
}

int get_named_values_scope(bool d) {
  if (d) {
    named_value_entry_t dummy = {strdup(""), get_type_from_name("void"), NULL};
    add_named_value(dummy);
  }
  return gen->named_values.count;
}

void generate_defer(defer_elem_t elem) {
  if (elem.t.kind != CLASS) {
    // maybe I should just do nothing in this case
    printf("Can only defer classes. Here: %s\n",
           elem.t.kind == BUILTIN ? "builtin" : "ptr");
    printf("\n");
    EXIT;
  }
  class_entry_t c = get_class_by_name(elem.t.name);
  if (does_method_exist(c, "destroy") < 0) {
    return; // no destroy method
  }
  method_t m = get_method_by_name(c, "destroy");
  LLVMValueRef fptr = fptr_from_method(m, c);
  LLVMTypeRef ftype = ftype_from_method(m, c);
  LLVMValueRef args[] = {elem.ptr};
  LLVMBuildCall2(gen->builder, ftype, fptr, args, 1, "");
}

void generate_defers2(int scope) {
  int start_index = gen->defers.count - 1;
  if (gen->defers.count > 0) {
    while (start_index > 0 && gen->defers.items[start_index].scope > scope) {
      start_index--;
    }
  }
  if (start_index > 0) {
    for (size_t i = start_index + 1; i < gen->defers.count; ++i) {
      printf("I is %ld\n", i);
      generate_defer(gen->defers.items[i]);
    }
  }
}

void generate_defers(int scope) {
  int old_count = gen->defers.count;
  if (gen->defers.count > 0) {
    while (gen->defers.count > 0 &&
           gen->defers.items[gen->defers.count - 1].scope > scope) {
      // generate_defer(gen->defers.items[gen->defers.count - 1]);
      gen->defers.count--;
    }
  }
  for (int i = gen->defers.count; i < old_count; i++) {
    generate_defer(gen->defers.items[i]);
  }
}

void reset_named_values_to_scope(int scope) {
  generate_defers(scope);
  for (size_t index = scope + 1; index < gen->named_values.count; index++) {
    int i = index;
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

type_t generate_templated_class_type(ast_t *type) {
  char *class_name = sv_to_cstr(type->as.type.name.lexeme);

  ast_t *template = NULL;
  for (size_t i = 0; i < gen->templates.count; ++i) {
    template_t temp = gen->templates.items[i];
    if (strcmp(temp.class_name, class_name) == 0) {
      template = temp.ast;
      break;
    }
  }
  if (template == NULL) {
    printf("Could not find templated class %s\n", class_name);
    EXIT;
  }

  ast_template_t temp = template->as.clazz.temp->as.temp;

  ptrs marked = {0};
  types ts = {0};
  for (size_t i = 0; i < temp.count; ++i) {
    void *mark = malloc(1);
    char *type_name = sv_to_cstr(
        temp.tempelems[i]->as.tempelem.type_iden->as.identifier.tok.lexeme);
    // TODO: actually handle interfaces constraints
    // alias it
    type_t *ref = malloc(sizeof(type_t));
    *ref = get_type_from_ast(type->as.type.inst_template->as.temp.tempelems[i]);
    type_t alias = {.kind = ALIAS,
                    .pointed_by = ref,
                    .name = type_name,
                    .ast = mark,
                    .type = NULL};
    da_append(&marked, mark);
    da_append(&ts, alias);
  }

  int inst_id = get_class_instance_index(class_name, ts);

  if (inst_id >= 0) {
    char instance_name[256];
    sprintf(instance_name, "%sZ%d", class_name, inst_id);
    type_t t = get_type_from_name(instance_name);
    free(class_name);
    return t;
  }

  for (size_t i = 0; i < gen->inst_classes.count; ++i) {
    inst_templ_class_t item = gen->inst_classes.items[i];
    if (strcmp(item.class_name, class_name) == 0) {
      inst_id++;
    }
  }

  for (size_t i = 0; i < ts.count; ++i) {
    add_type(ts.items[i]);
  }

  types instance_types = {0};
  for (size_t i = 0; i < ts.count; ++i) {
    da_append(&instance_types, ts.items[i]);
  }

  type_t t = add_class_templated(template);
  inst_templ_class_t instance = {strdup(class_name), instance_types};
  da_append(&gen->inst_classes, instance);

  generate_classdef(template);

  for (size_t j = 0; j < marked.count; j++) {
    void *ptr = marked.items[j];
    for (int i = gen->types.count - 1; i >= 0; --i) {
      type_t current = gen->types.items[i];
      if (current.kind == ALIAS && current.ast == ptr) {
        da_remove(&gen->types, i);
        break;
      }
    }
    free(ptr);
  }

  return t;
}

type_t get_type_from_ast(ast_t *type) {
  if (type->as.type.ptr_n == 0) {
    if (type->as.type.is_template) {
      return generate_templated_class_type(type);
    }
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

bool is_file_included(const char *path) {
  for (size_t i = 0; i < gen->included_files.count; ++i) {
    if (strcmp(path, gen->included_files.items[i]) == 0) {
      return true;
    }
  }
  return false;
}

bool is_include_std(ast_t *include) {
  if (include->kind == AST_IDENTIFIER) {
    return sv_eq(SV("std"), include->as.identifier.tok.lexeme);
  }
  if (include->kind != AST_BINOP) {
    printf("Wrong include stmt: ");
    dump_ast(include);
  }
  return is_include_std(include->as.binop.lhs);
}

void generate_include(ast_t *include) {
  char *postfix = get_include_postfix(include->as.include_dir.expr);
  char include_directory[PATH_MAX - 256] = {0};
  char include_path[PATH_MAX] = {0};
  if (is_include_std(include->as.include_dir.expr)) {
    char *home = getenv("HOME");
    sprintf(include_directory, "%s/Documents/Unilang/stdlib", home);
  } else {
    // Try from current directory
    void *res = getcwd(include_directory, sizeof(include_directory));
    if (res == NULL) {
      printf("WTF ???\n");
    }
  }
  sprintf(include_path, "%s/%s.ul", include_directory, postfix);
  printf("INCLUDING %s\n", include_path);
  // temporary
  free(postfix);
  if (!is_file_included(include_path)) {
    da_append(&gen->included_files, strdup(include_path));
    FILE *f = fopen(include_path, "r");
    if (f == NULL) {
      printf("Could not include %s\n", include_path);
      EXIT;
    }
    string_view_t s = from_file(f);
    fclose(f);
    lexer_t l = new_unilang_lexer();
    l.remaining = s;
    l.current_loc = (location_t){include_path, 1, 1, false};
    int worked = 0;
    ast_t *prog = parse_program(&l, &worked);
    if (!worked) {
      printf("Could not include %s\n", include_path);
      EXIT;
    }
    // TODO: generate all entries from it !
    // no need to actually geenrate ir because the library will be linked
    ast_program_t p = prog->as.program;
    for (size_t i = 0; i < prog->as.program.elem_count; ++i) {
      ast_t *decl = p.elems[i];
      if (decl->kind == AST_FUNDEF) {
        function_entry_t entry = entry_from_fundef(decl);
        add_function_from_entry(entry);
        add_function(entry);
      } else if (decl->kind == AST_CLASS) {
        generate_classdef_for_include(decl);
      } else if (decl->kind == AST_INCLUDE_DIR) {
        // TODO: check for circular includes
        generate_include(decl);
      } else if (decl->kind == AST_INTERFACE) {
        generate_interface(decl);
      } else {
        TODO;
        printf("TODO: decl type %d not supported in include !\n", decl->kind);
        EXIT;
      }
    }
  }
}

void generate_interface(ast_t *interface) {
  char *name = sv_to_cstr(interface->as.interface.name.lexeme);
  char *type = sv_to_cstr(interface->as.interface.type.lexeme);
  interface_entry_t entry = {name, type, (functions){0}};
  void *marker = malloc(1);
  type_t tmp_type = {.name = type,
                     .pointed_by = marker,
                     .kind = INTERFACE,
                     .ast = NULL,
                     .type = NULL};
  add_type(tmp_type);
  for (size_t i = 0; i < interface->as.interface.protos_count; ++i) {
    function_entry_t f = entry_from_fundef(interface->as.interface.protos[i]);
    da_append(&entry.protos, f);
  }
  da_append(&gen->interfaces, entry);
  for (int i = gen->types.count - 1; i >= 0; --i) {
    type_t current = gen->types.items[i];
    if (current.pointed_by == marker) {
      da_remove(&gen->types, i);
      break;
    }
  }

  free(marker);
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
    generate_classdef_for_include(decl);
    if (decl->as.clazz.temp == NULL) {
      generate_classdef(decl);
    }
  } break;
  case AST_CT_CTE: {
    generate_ct_cte(decl);
  } break;
  case AST_INCLUDE_DIR: {
    generate_include(decl);
  } break;
  case AST_INTERFACE: {
    generate_interface(decl);
  } break;
  default: {
    printf("Unexpected declaration kind: ");
    dump_ast(decl);
    printf("\n");
    EXIT;
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

type_t get_aliased_with_name(const char *name, void *ref) {
  int start_index = -1;

  // for (int i = gen->types.count - 1; i>= 0; --i) {
  //   type_t t = gen->types.items[i];
  //   if (t.kind != TEMPLATED) {
  //     continue;
  //   }
  //   if (strcmp(t.name, name) == 0) {
  //    start_index = i;
  //   }
  // }
  if (start_index < 0) {
    printf("No start_index found !\n");
    start_index = gen->types.count - 1;
  }
  // for (int i = 0; i < gen->types.count; ++i) {
  for (int i = start_index; i >= 0; --i) {
    type_t t = gen->types.items[i];
    if (t.kind != ALIAS) {
      continue;
    }
    if (strcmp(t.name, name) == 0) {
      if (t.pointed_by == ref) {
        continue;
      }
      return t;
    }
  }
  printf("Could not retrieve alias type with name %s in current context.\n",
         name);
  printf("Current context:\n");
  print_types();
  EXIT;
}

LLVMTypeRef type_to_llvm(type_t t) {
  if (t.kind == TEMPLATED) {
    return type_to_llvm(get_aliased_with_name(t.name, NULL));
  }
  if (t.kind == AST) {
    return type_to_llvm(get_type_from_ast(t.ast));
  }
  if (t.kind == ALIAS) {
    return type_to_llvm(*t.pointed_by);
  }
  if (t.kind == PTR) {
    return LLVMPointerType(type_to_llvm(*t.pointed_by), 0);
  }
  return t.type;
}

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
    LLVMValueRef ptr = LLVMBuildAlloca(gen->builder, type_to_llvm(t), "ptr");
    LLVMBuildStore(gen->builder, arg, ptr);
    named_value_entry_t nv = {name, t, ptr};
    da_append(&gen->named_values, nv);
  }
}

void generate_compound_for_fun(ast_t *compound) {
  ast_compound_t body = compound->as.compound;
  for (size_t i = 0; i < body.elem_count; ++i) {
    generate_statement(body.elems[i]);
  }
}

void generate_compound(ast_t *compound) {
  int scope = get_named_values_scope(0);
  ast_compound_t body = compound->as.compound;
  for (size_t i = 0; i < body.elem_count; ++i) {
    generate_statement(body.elems[i]);
  }
  reset_named_values_to_scope(scope);
}

void add_function_from_entry(function_entry_t entry) {
  LLVMTypeRef ftype = ftype_from_entry(entry);
  LLVMAddFunction(gen->module, entry.name, ftype);
}

void generate_function_body(function_entry_t entry, ast_t *body) {
  if (gen->current_function == NULL) {
    gen->current_function = malloc(sizeof(function_entry_t));
  }
  *gen->current_function = entry;
  add_function_from_entry(entry);
  LLVMValueRef fptr = fptr_from_entry(entry);

  LLVMBasicBlockRef bb_entry =
      LLVMAppendBasicBlockInContext(gen->context, fptr, "entry");

  LLVMPositionBuilderAtEnd(gen->builder, bb_entry);
  gen->last_bb = bb_entry;

  int scope = get_named_values_scope(0);
  gen->current_function_scope = scope;

  push_params(entry, fptr);

  generate_compound_for_fun(body);

  reset_named_values_to_scope(scope);

  if (LLVMGetBasicBlockTerminator(gen->last_bb) == NULL) {
    if (strcmp(entry.return_type.name, "void") == 0) {
      LLVMBuildRetVoid(gen->builder);
    } else {
      LLVMBuildUnreachable(gen->builder);
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
  EXIT;
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
  EXIT;
}

int get_class_instance_index(const char *class_name, types ts) {
  size_t instance_id = -1;
  for (size_t i = 0; i < gen->inst_classes.count; ++i) {
    inst_templ_class_t item = gen->inst_classes.items[i];
    if (strcmp(item.class_name, class_name)) {
      continue;
    }
    bool found = true;
    for (size_t j = 0; j < item.ts.count; ++j) {
      type_t got = ts.items[j];
      type_t actual = item.ts.items[j];
      if (!are_types_equal(got, actual)) {
        found = false;
        break;
      }
    }
    instance_id++;
    if (found) {
      return instance_id;
    }
  }
  return -1; // Not found
}

class_entry_t get_class_by_name(const char *name) {
  for (size_t i = 0; i < gen->classes.count; i++) {
    if (strcmp(gen->classes.items[i].name, name) == 0) {
      return gen->classes.items[i];
    }
  }
  printf("No class named %s found.\n", name);
  EXIT;
}

type_t get_return_type(ast_t *funcall) {
  if (funcall->as.funcall.called->kind == AST_IDENTIFIER) {
    char *funname =
        sv_to_cstr(funcall->as.funcall.called->as.identifier.tok.lexeme);
    if (does_type_exist(funname)) {
      type_t res = get_type_from_name(funname);
      free(funname);
      return res;
    }
    free(funname);
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
      EXIT;
    }
    type_t t = t_of_expr(called->as.binop.lhs);
    if (t.kind == PTR) {
      t = dereference_type(t);
    }
    if (t.kind != CLASS) {
      printf("error: cannot call non-class method\n");
      EXIT;
    }

    class_entry_t cdef = get_class_by_name(t.name);
    if (called->as.binop.rhs->kind != AST_IDENTIFIER) {
      printf("error: cannot call method with non-identifier name\n");
      EXIT;
    }
    char *name = sv_to_cstr(called->as.binop.rhs->as.identifier.tok.lexeme);
    method_t m = get_method_by_name(cdef, name);
    return get_type_used_in_class(cdef, m.return_type);
  }
  printf("Unreachable 1\n");
  EXIT;
}

int get_matching_constructor(const char *class_name, types arg_types) {
  // Get the class entry
  class_entry_t cdef = get_class_by_name(class_name);

  // For each constructor
  for (size_t i = 0; i < cdef.constructors.count; i++) {
    constructor_t c = cdef.constructors.items[i];

    // Check argument count matches
    if (c.arg_types.count != arg_types.count) {
      continue;
    }

    // Check if all arguments can match (either exactly or through implicit
    // casting)
    bool can_match = true;
    for (size_t j = 0; j < c.arg_types.count; j++) {
      type_t expected = get_type_used_in_class(cdef, c.arg_types.items[j]);
      type_t provided = arg_types.items[j];

      if (are_types_equal(expected, provided)) {
        continue; // Exact match
      }

      // Try implicit casting rules
      if (expected.kind == CLASS) {
        // Check if there's a constructor that can take the provided type
        class_entry_t target_class = get_class_by_name(expected.name);
        int constructor_index = get_constructor_with_single_arg_matching_type(
            target_class, type_to_llvm(provided));
        if (constructor_index >= 0) {
          continue; // Can construct from this type
        }
      }
      // Check numeric type casting
      if (is_integer_type(expected) && is_integer_type(provided)) {
        continue; // Can cast between integer types
      }
      // Check pointer type casting
      if (expected.kind == PTR && provided.kind == PTR) {
        continue; // Can cast between pointer types
      }
      // Check float to int and int to float
      if ((strcmp(expected.name, "float") == 0 && is_integer_type(provided)) ||
          (is_integer_type(expected) && strcmp(provided.name, "float") == 0)) {
        continue;
      }
      // If we get here, we can't cast this argument
      can_match = false;
      break;
    }
    if (can_match) {
      return i;
    }
  }
  return -1;
}

LLVMValueRef generate_funcall(ast_t *funcall) {
  if (funcall->as.funcall.called->kind == AST_IDENTIFIER) {
    char *name =
        sv_to_cstr(funcall->as.funcall.called->as.identifier.tok.lexeme);
    if (does_type_exist(name)) {
      // try and find a suitable constructor !
      types ts = {0};
      for (size_t i = 0; i < funcall->as.funcall.arg_count; ++i) {
        type_t t = t_of_expr(funcall->as.funcall.args[i]);
        da_append(&ts, t);
      }
      int cons = get_matching_constructor(name, ts);
      da_free(ts);
      if (cons < 0) {
        printf("Could not find matching constructor for %s(", name);
        for (size_t i = 0; i < ts.count; ++i) {
          if (i > 0) {
            printf(", ");
          }
          print_type(ts.items[i]);
        }
        printf(")\n");
        EXIT;
      }
      // printf("TRYING TO CALL CONSTRUCTOR !\n");
      // EXIT;
      class_entry_t cdef = get_class_by_name(name);
      constructor_t c = cdef.constructors.items[cons];
      LLVMValueRef ptr;
      if (gen->current_ptr != NULL) {
        ptr = gen->current_ptr;
      } else {
        type_t self_type = get_type_from_name(name);
        ptr = LLVMBuildAlloca(gen->builder, type_to_llvm(self_type), "");
      }
      lvalues args = {0};
      da_append(&args, ptr);
      for (size_t i = 0; i < funcall->as.funcall.arg_count; ++i) {
        LLVMValueRef arg = generate_expression(funcall->as.funcall.args[i]);
        da_append(&args, arg);
      }
      LLVMValueRef fptr = fptr_from_constructor(c, cdef, cons);
      LLVMTypeRef ftype = ftype_from_constructor(c, cdef);
      LLVMBuildCall2(gen->builder, ftype, fptr, args.items, args.count, "");
      da_free(args);
      type_t t = t_of_expr(funcall);
      return LLVMBuildLoad2(gen->builder, type_to_llvm(t), ptr, "");
    }
    function_entry_t entry = get_global_function(funcall->as.funcall.called);
    LLVMValueRef called = LLVMGetNamedFunction(gen->module, entry.name);
    lvalues args = {0};
    if (funcall->as.funcall.arg_count != entry.arg_names.count &&
        !entry.is_var_args) {
      printf("error: function %s takes %ld arguments but %ld were given.\n",
             entry.name, entry.arg_names.count, funcall->as.funcall.arg_count);
      EXIT;
    }
    for (size_t i = 0; i < funcall->as.funcall.arg_count; ++i) {
      type_t t = entry.arg_types.items[i];
      ast_t *arg = funcall->as.funcall.args[i];
      type_t original_type = t_of_expr(arg);
      LLVMValueRef arg_val = generate_expression(arg);
      if (i < entry.arg_names.count) {
        if (!are_types_equal(original_type, t)) {
          arg_val = generate_cast(arg_val, original_type, t);
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
      EXIT;
    }
    type_t t = t_of_expr(called->as.binop.lhs);
    t = sanitize_type(t);
    if (t.kind == PTR) {
      t = dereference_type(t);
    }
    if (t.kind != CLASS) {
      printf("error: cannot call non-class method of type ");
      print_type(t);
      printf("\n");
      dump_ast(called);

      EXIT;
    }
    class_entry_t cdef = get_class_by_name(t.name);
    if (called->as.binop.rhs->kind != AST_IDENTIFIER) {
      printf("error: cannot call method with non-identifier name\n");
      EXIT;
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
      EXIT;
    }
    for (size_t i = 0; i < m.arg_names.count; ++i) {
      ast_t *expr = funcall->as.funcall.args[i];
      type_t arg_t = get_type_used_in_class(cdef, m.arg_types.items[i]);
      LLVMValueRef arg = generate_expression(expr);
      type_t original_type = t_of_expr(expr);
      if (!are_types_equal(arg_t, original_type)) {
        arg = generate_cast(arg, original_type, arg_t);
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
  printf("Unreachable 2\n");
  dump_ast(funcall);
  printf("\n");
  EXIT;
}

int get_index_of_field(const char *field_name, class_entry_t cdef) {
  for (size_t i = 0; i < cdef.members.count; i++) {
    if (strcmp(field_name, cdef.members.items[i].name) == 0) {
      return i;
    }
  }
  printf("No field %s in class %s.\n", field_name, cdef.name);
  EXIT;
}

int get_binop_method_index(token_kind_t op, class_entry_t cdef) {
  char *name;
  switch (op) {
  case PLUS: {
    name = "op_add";
  } break;
  case MINUS: {
    name = "op_sub";
  } break;
  case MULT: {
    name = "op_mul";
  } break;
  case EQ: {
    name = "op_eq";
  } break;
  default: {
    printf("%s:%d TODO: other op kinds\n", __FILE__, __LINE__);
    EXIT;
  }
  }
  return does_method_exist(cdef, name);
}

bool is_cmp(token_kind_t k) {
  switch (k) {
  case EQ:
  case DIFF:
  case LEQ:
  case GEQ:
  case LT:
  case GT:
  case AND:
  case OR:
    return true;
  default:
    return false;
  }
}

type_t sanitize_type(type_t t) {
  if (t.kind == PTR) {
    return get_ptr_of(sanitize_type(*t.pointed_by));
  }
  if (t.kind == ALIAS) {
    return sanitize_type(*t.pointed_by);
  }
  if (t.kind == TEMPLATED) {
    type_t res = get_aliased_with_name(t.name, NULL);
    return sanitize_type(res);
  }
  if (t.kind == AST) {
    printf("AST KIND ");
    dump_ast(t.ast);
    printf("\n");
    return sanitize_type(get_type_from_ast(t.ast));
  }
  return t;
}

type_t t_of_expr_unsafe(ast_t *expr) {
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
        EXIT;
      }
      class_entry_t c = get_class_by_name(lhs.name);
      if (expr->as.binop.rhs->kind != AST_IDENTIFIER) {
        printf("Expected identifier for field name\n");
        EXIT;
      }
      char *field_name =
          sv_to_cstr(expr->as.binop.rhs->as.identifier.tok.lexeme);
      int index = get_index_of_field(field_name, c);
      free(field_name);
      return sanitize_type(c.members.items[index].type);
    }
    if (is_cmp(expr->as.binop.op.kind)) {
      return get_type_from_name("bool");
    }
    type_t lt = t_of_expr(expr->as.binop.lhs);
    type_t rt = t_of_expr(expr->as.binop.rhs);
    if (lt.kind == CLASS) {
      class_entry_t cdef = get_class_by_name(lt.name);
      int index = get_binop_method_index(expr->as.binop.op.kind, cdef);
      if (index < 0) {
        printf("No method for '" SF "' binop in class %s\n",
               SA(expr->as.binop.op.lexeme), lt.name);
        EXIT;
      }
      method_t m = cdef.methods.items[index];
      return get_type_used_in_class(cdef, m.return_type);
    }
    if (type_to_llvm(lt) == type_to_llvm(rt)) {
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
      EXIT;
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
    if (expr->as.unop.op.kind == BIT_AND) {
      type_t t = t_of_expr(expr->as.unop.operand);
      return get_ptr_of(t);
    }
    // TODO
    return t_of_expr(expr->as.unop.operand);
  } break;
  case AST_AS_DIR:
  case AST_NEW_DIR: {
    type_t t = get_type_from_ast(expr->as.as_dir.type);
    return t;
  } break;
  case AST_INDEX: {
    return dereference_type(t_of_expr(expr->as.index.subscripted));
  }
  case AST_CHARLIT: {
    return get_type_from_name("char");
  } break;
  case AST_FLOATLIT: {
    return get_type_from_name("float");
  } break;
  case AST_SIZE_DIR: {
    return get_type_from_llvm(LLVMTypeOf(
        LLVMSizeOf(type_to_llvm(get_type_from_ast(expr->as.size_dir.type)))));
  } break;
  default: {
    printf("%s:%d TODO: get type of expression %d\n", __FILE__, __LINE__,
           expr->kind);
    EXIT;
  }
  }
}

// type_t t_of_expr(ast_t *ast) { return sanitize_type(t_of_expr_unsafe(ast));
// }

void add_defer(defer_elem_t d) {
  for (size_t i = 0; i < gen->defers.count; ++i) {
    if (d.ptr == gen->defers.items[i].ptr)
      return;
  }
  da_append(&gen->defers, d);
}

LLVMValueRef generate_binop(ast_t *binop) {
  LLVMValueRef lhs = generate_expression(binop->as.binop.lhs);
  LLVMValueRef rhs = generate_expression(binop->as.binop.rhs);
  type_t lt = t_of_expr(binop->as.binop.lhs);
  type_t rt = t_of_expr(binop->as.binop.rhs);
  if (lt.kind == CLASS) {
    type_t glob_type = t_of_expr(binop);
    if (glob_type.kind == CLASS) {
      LLVMValueRef ptr = get_lm_pointer(binop);
      defer_elem_t entry = {get_named_values_scope(1), glob_type, ptr};
      if (binop->as.binop.op.kind != ACCESS) {
        add_defer(entry);
      }
      LLVMValueRef res = LLVMBuildLoad2(gen->builder, glob_type.type, ptr, "");
      int is_new = gen->is_new;
      gen->is_new = 0;
      res = generate_cast(res, glob_type, glob_type);
      gen->is_new = is_new;
      return res;
    }
    class_entry_t cdef = get_class_by_name(lt.name);
    int index = get_binop_method_index(binop->as.binop.op.kind, cdef);
    if (index < 0) {
      printf("No method for '" SF "' binop in class %s\n",
             SA(binop->as.binop.op.lexeme), lt.name);
      EXIT;
    }
    gen->current_ptr = NULL;
    method_t m = cdef.methods.items[index];
    int is_new = gen->is_new;
    gen->is_new = 0;
    LLVMValueRef left = get_lm_pointer(binop->as.binop.lhs);
    LLVMValueRef right = generate_expression(binop->as.binop.rhs);
    right = generate_cast(right, rt, lt);
    gen->is_new = is_new;
    LLVMValueRef fptr = fptr_from_method(m, cdef);
    LLVMTypeRef ftype = ftype_from_method(m, cdef);
    LLVMValueRef args[] = {left, right};
    return LLVMBuildCall2(gen->builder, ftype, fptr, args, 2, "");
  }
  bool is_ptr = false;
  if (type_to_llvm(lt) != type_to_llvm(rt)) {
    if (lt.kind == PTR) {
      is_ptr = true;
      lhs = generate_cast(lhs, lt, rt);
    } else if (rt.kind == PTR) {
      is_ptr = true;
      rhs = generate_cast(rhs, rt, lt);
    } else if (LLVMSizeOf(type_to_llvm(lt)) > LLVMSizeOf(type_to_llvm(rt))) {
      rhs = generate_cast(rhs, rt, lt);
    } else {
      lhs = generate_cast(lhs, lt, rt);
    }
  }
  LLVMValueRef res;
  switch (binop->as.binop.op.kind) {
  case PLUS: {
    if (strcmp(lt.name, "float") == 0) {
      res = LLVMBuildFAdd(gen->builder, lhs, rhs, "");
    } else {
      res = LLVMBuildAdd(gen->builder, lhs, rhs, "");
    }
  } break;
  case MINUS: {
    if (strcmp(lt.name, "float") == 0) {
      res = LLVMBuildFSub(gen->builder, lhs, rhs, "");
    } else {
      res = LLVMBuildSub(gen->builder, lhs, rhs, "");
    }
  } break;
  case MULT: {
    if (strcmp(lt.name, "float") == 0) {
      res = LLVMBuildFMul(gen->builder, lhs, rhs, "");
    } else {
      res = LLVMBuildMul(gen->builder, lhs, rhs, "");
    }
  } break;
  case DIV: {
    if (strcmp(lt.name, "float") == 0) {
      res = LLVMBuildFDiv(gen->builder, lhs, rhs, "");
    } else {
      res = LLVMBuildSDiv(gen->builder, lhs, rhs, "");
    }
  } break;
  case MODULO: {
    if (strcmp(lt.name, "float") == 0) {
      printf("TODO: no %% for floats\n");
      EXIT;
    }
    res = LLVMBuildSRem(gen->builder, lhs, rhs, "");
  } break;
  case LT: {
    if (strcmp(lt.name, "float") == 0) {
      LLVMTypeRef t = get_type_from_name("int").type;
      LLVMValueRef sub = LLVMBuildFSub(gen->builder, lhs, rhs, "");
      sub = LLVMBuildFPToSI(gen->builder, sub, t, "");
      lhs = sub;
      rhs = LLVMConstInt(t, 0, 0);
    }
    res = LLVMBuildICmp(gen->builder, LLVMIntSLT, lhs, rhs, "");
  } break;
  case GT: {
    if (strcmp(lt.name, "float") == 0) {
      LLVMTypeRef t = get_type_from_name("int").type;
      LLVMValueRef sub = LLVMBuildFSub(gen->builder, lhs, rhs, "");
      sub = LLVMBuildFPToSI(gen->builder, sub, t, "");
      lhs = sub;
      rhs = LLVMConstInt(t, 0, 0);
    }
    res = LLVMBuildICmp(gen->builder, LLVMIntSGT, lhs, rhs, "");
  } break;
  case LEQ: {
    if (strcmp(lt.name, "float") == 0) {
      LLVMTypeRef t = get_type_from_name("int").type;
      LLVMValueRef sub = LLVMBuildFSub(gen->builder, lhs, rhs, "");
      sub = LLVMBuildFPToSI(gen->builder, sub, t, "");
      lhs = sub;
      rhs = LLVMConstInt(t, 0, 0);
    }
    res = LLVMBuildICmp(gen->builder, LLVMIntSLE, lhs, rhs, "");
  } break;
  case GEQ: {
    if (strcmp(lt.name, "float") == 0) {
      LLVMTypeRef t = get_type_from_name("int").type;
      LLVMValueRef sub = LLVMBuildFSub(gen->builder, lhs, rhs, "");
      sub = LLVMBuildFPToSI(gen->builder, sub, t, "");
      lhs = sub;
      rhs = LLVMConstInt(t, 0, 0);
    }
    res = LLVMBuildICmp(gen->builder, LLVMIntSGE, lhs, rhs, "");
  } break;
  case EQ: {
    if (strcmp(lt.name, "float") == 0) {
      LLVMTypeRef t = get_type_from_name("int").type;
      LLVMValueRef sub = LLVMBuildFSub(gen->builder, lhs, rhs, "");
      sub = LLVMBuildFPToSI(gen->builder, sub, t, "");
      lhs = sub;
      rhs = LLVMConstInt(t, 0, 0);
    }
    res = LLVMBuildICmp(gen->builder, LLVMIntEQ, lhs, rhs, "");
  } break;
  case DIFF: {
    if (strcmp(lt.name, "float") == 0) {
      LLVMTypeRef t = get_type_from_name("int").type;
      LLVMValueRef sub = LLVMBuildFSub(gen->builder, lhs, rhs, "");
      sub = LLVMBuildFPToSI(gen->builder, sub, t, "");
      lhs = sub;
      rhs = LLVMConstInt(t, 0, 0);
    }
    res = LLVMBuildICmp(gen->builder, LLVMIntNE, lhs, rhs, "");
  } break;
  case AND: {
    type_t b = get_type_from_name("bool");
    lhs = generate_cast(lhs, lt, b);
    rhs = generate_cast(rhs, rt, b);
    res = LLVMBuildAnd(gen->builder, lhs, rhs, "");
  } break;
  case OR: {
    type_t b = get_type_from_name("bool");
    lhs = generate_cast(lhs, lt, b);
    rhs = generate_cast(rhs, rt, b);
    res = LLVMBuildOr(gen->builder, lhs, rhs, "");
  } break;
  default: {
    printf("%s:%d TODO: generate binary operation %d\n", __FILE__, __LINE__,
           binop->as.binop.op.kind);
    EXIT;
  }
  }
  if (is_ptr) {
    res = generate_cast(res, lt, get_ptr_of(get_type_from_name("void")));
  }
  return res;
}

LLVMValueRef generate_access(ast_t *binop) {
  type_t t = t_of_expr(binop);
  LLVMValueRef ptr = get_lm_pointer(binop);
  LLVMValueRef res = LLVMBuildLoad2(gen->builder, type_to_llvm(t), ptr, "");
  return res;
}

bool is_type_int(type_t t) {
  return t.kind == BUILTIN && strcmp(t.name, "void") != 0;
}

int get_named_value(char *name) {
  for (size_t i = 0; i < gen->named_values.count; ++i) {
    int index = gen->named_values.count - i - 1;
    char *s1 = gen->named_values.items[index].name;
    if (strlen(s1) > 0) {
      if (strcmp(gen->named_values.items[index].name, name) == 0) {
        return index;
      }
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
    EXIT;
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
  if (expr->as.unop.op.kind == BIT_AND) {
    return get_lm_pointer(expr->as.unop.operand);
  }
  if (expr->as.unop.op.kind == MINUS) {
    type_t t = t_of_expr(expr->as.unop.operand);
    if (strcmp(t.name, "float") == 0) {
      return LLVMBuildFSub(gen->builder,
                           LLVMConstReal(get_type_from_name("float").type, 0),
                           generate_expression(expr->as.unop.operand), "");
    }
    if (t.kind != BUILTIN) {
      printf("Cannot '-' non builtin types for now\n");
      EXIT;
    }
    return LLVMBuildSub(gen->builder, LLVMConstInt(t.type, 0, 0),
                        generate_expression(expr->as.unop.operand), "");
  }
  if (expr->as.unop.op.kind == NOT) {
    type_t t = t_of_expr(expr->as.unop.operand);
    type_t bool_t = get_type_from_name("bool");
    LLVMValueRef e = generate_expression(expr->as.unop.operand);
    if (!are_types_equal(t, bool_t)) {
      e = generate_cast(e, t, bool_t);
    }
    return LLVMBuildNot(gen->builder, e, "");
  }
  printf("%s:%d TODO: generate_unop.\n", __FILE__, __LINE__);
  EXIT;
}

LLVMValueRef generate_index(ast_t *expr) {
  ast_t *lhs = expr->as.index.subscripted;
  ast_t *rhs = expr->as.index.index;
  type_t base = t_of_expr(lhs);
  type_t actual = dereference_type(base);
  LLVMValueRef lhs_ptr = generate_expression(lhs);
  LLVMValueRef rhs_val = generate_expression(rhs);
  LLVMValueRef res =
      LLVMBuildGEP2(gen->builder, actual.type, lhs_ptr, &rhs_val, 1, "");
  res = LLVMBuildLoad2(gen->builder, actual.type, res, "");
  return res;
}

LLVMValueRef generate_as_dir(ast_t *expr) {
  type_t to_cast = t_of_expr(expr);
  LLVMValueRef lm = get_lm_pointer(expr);
  return LLVMBuildLoad2(gen->builder, type_to_llvm(to_cast), lm, "");
}

LLVMValueRef generate_new_dir(ast_t *expr) {
  type_t to_cast = t_of_expr(expr);
  type_t original = t_of_expr(expr->as.new_dir.expr);
  printf("Type to cast is ");
  print_type(original);
  printf(" and original is ");
  print_type(to_cast);
  printf("\n\n");
  LLVMValueRef current_ptr = gen->current_ptr;
  int is_new = gen->is_new;
  gen->current_ptr = NULL;
  LLVMValueRef res = generate_expression(expr->as.new_dir.expr);
  gen->is_new = 1;
  res = generate_cast_no_check(res, original, to_cast);
  gen->current_ptr = current_ptr;
  gen->is_new = is_new;
  return res;
}

LLVMValueRef generate_floatlit(token_t tok) {
  char *s = sv_to_cstr(tok.lexeme);
  LLVMValueRef res = LLVMConstRealOfString(get_type_from_name("float").type, s);
  free(s);
  return res;
}

LLVMValueRef generate_expression(ast_t *expr) {
  switch (expr->kind) {
  case AST_INTLIT: {
    return generate_intlit(expr->as.intlit.tok);
  } break;
  case AST_FLOATLIT: {
    return generate_floatlit(expr->as.intlit.tok);
  }
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
      printf("Identifier %s not declared in this scope 2 (%d) \n", name, index);
      EXIT;
    }
    named_value_entry_t entry = gen->named_values.items[index];
    LLVMValueRef ptr = entry.value;
    type_t t = entry.t;
    LLVMValueRef res = LLVMBuildLoad2(gen->builder, type_to_llvm(t), ptr, name);
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
  case AST_INDEX: {
    return generate_index(expr);
  }
  case AST_AS_DIR: {
    return generate_as_dir(expr);
  }
  case AST_NEW_DIR: {
    return generate_new_dir(expr);
  }
  case AST_CHARLIT: {
    string_view_t lit = expr->as.charlit.tok.lexeme;
    lit.contents++;
    lit.length -= 2;
    char *cstr = sv_to_cstr(lit);
    char *cstr2 = unescape_string(cstr);
    char c = cstr2[0];
    free(cstr);
    free(cstr2);
    LLVMValueRef res = LLVMConstInt(get_type_from_name("char").type, c, 0);
    return res;
  }
  case AST_SIZE_DIR: {
    return LLVMSizeOf(type_to_llvm(get_type_from_ast(expr->as.size_dir.type)));
  }
  default:
    printf("%s:%d TODO: generate_expression %d\n", __FILE__, __LINE__,
           expr->kind);
    EXIT;
  }
}

void generate_ifstmt(ast_t *if_stmt) {
  type_t t = t_of_expr(if_stmt->as.if_stmt.cond);
  type_t bool_t = get_type_from_name("bool");
  LLVMValueRef cond = generate_expression(if_stmt->as.if_stmt.cond);
  if (!are_types_equal(t, bool_t)) {
    cond = generate_cast(cond, t, bool_t);
  }
  function_entry_t entry = *gen->current_function;
  LLVMValueRef fptr = fptr_from_entry(entry);

  LLVMBasicBlockRef then_block = LLVMAppendBasicBlock(fptr, "then");
  LLVMBasicBlockRef else_block = LLVMAppendBasicBlock(fptr, "else");
  LLVMBasicBlockRef merge_block = LLVMAppendBasicBlock(fptr, "merge");
  LLVMBuildCondBr(gen->builder, cond, then_block, else_block);
  LLVMPositionBuilderAtEnd(gen->builder, then_block);
  gen->last_bb = then_block;
  int scope = get_named_values_scope(1);
  generate_statement(if_stmt->as.if_stmt.body);
  reset_named_values_to_scope(scope);

  if (LLVMGetBasicBlockTerminator(gen->last_bb) == NULL) {
    LLVMBuildBr(gen->builder, merge_block);
  }
  LLVMPositionBuilderAtEnd(gen->builder, else_block);
  gen->last_bb = else_block;
  if (if_stmt->as.if_stmt.other_body) {
    scope = get_named_values_scope(1);
    generate_statement(if_stmt->as.if_stmt.other_body);
    reset_named_values_to_scope(scope);
    if (LLVMGetBasicBlockTerminator(gen->last_bb) == NULL) {
      LLVMBuildBr(gen->builder, merge_block);
    }
  } else {
    if (LLVMGetBasicBlockTerminator(else_block) == NULL) {
      LLVMBuildBr(gen->builder, merge_block);
    }
  }
  LLVMPositionBuilderAtEnd(gen->builder, merge_block);
  gen->last_bb = merge_block;
}
void generate_whilestmt(ast_t *while_stmt) {
  ast_t *cond = while_stmt->as.while_stmt.cond;
  ast_t *body = while_stmt->as.while_stmt.body;
  LLVMValueRef fptr = fptr_from_entry(*gen->current_function);
  LLVMBasicBlockRef bb_cond = LLVMAppendBasicBlock(fptr, "cond");
  LLVMBasicBlockRef bb_body = LLVMAppendBasicBlock(fptr, "body");
  LLVMBasicBlockRef bb_after = LLVMAppendBasicBlock(fptr, "after");

  LLVMBuildBr(gen->builder, bb_cond);

  LLVMPositionBuilderAtEnd(gen->builder, bb_cond);
  gen->last_bb = bb_cond;
  LLVMValueRef cond_expr = generate_expression(cond);
  type_t t = t_of_expr(cond);
  type_t bool_t = get_type_from_name("bool");
  if (LLVMTypeOf(cond_expr) != type_to_llvm(bool_t)) {
    cond_expr = generate_cast(cond_expr, t, bool_t);
  }
  LLVMBuildCondBr(gen->builder, cond_expr, bb_body, bb_after);
  LLVMPositionBuilderAtEnd(gen->builder, bb_body);
  gen->last_bb = bb_body;
  int scope = get_named_values_scope(1);
  generate_statement(body);
  reset_named_values_to_scope(scope);
  if (LLVMGetBasicBlockTerminator(gen->last_bb) == NULL) {
    LLVMBuildBr(gen->builder, bb_cond);
  }
  LLVMPositionBuilderAtEnd(gen->builder, bb_after);
  gen->last_bb = bb_after;
}

type_t t_from_cdef(class_entry_t cdef) {
  printf("Creating class type for %s\n", cdef.name);
  LLVMTypeRef str = LLVMStructCreateNamed(gen->context, cdef.name);
  ltypes mems = {0};
  for (size_t i = 0; i < cdef.members.count; ++i) {
    member_t m = cdef.members.items[i];
    da_append(&mems, type_to_llvm(m.type));
  }
  LLVMStructSetBody(str, mems.items, mems.count, 0);
  da_free(mems);
  type_t t = {.kind = CLASS,
              .type = str,
              .name = strdup(cdef.name),
              .pointed_by = NULL,
              .interface = NULL,
              .ast = NULL};
  LLVMDumpType(str);
  printf("\n");
  return t;
}

LLVMTypeRef ftype_from_constructor(constructor_t constructor,
                                   class_entry_t cdef) {
  ltypes ts = {0};
  type_t self = get_type_from_name(cdef.name);
  da_append(&ts, type_to_llvm(get_ptr_of(self)));
  for (size_t j = 0; j < constructor.arg_names.count; j++) {
    type_t t = constructor.arg_types.items[j];
    t = get_type_used_in_class(cdef, t);
    da_append(&ts, type_to_llvm(t));
  }
  LLVMTypeRef ftype =
      LLVMFunctionType(get_type_from_name("void").type, ts.items, ts.count, 0);
  da_free(ts);
  return ftype;
}

LLVMValueRef declare_constructor(class_entry_t cdef, size_t i) {
  if (i > cdef.constructors.count) {
    printf("Could not generate %s constructor %ld out of %ld\n", cdef.name, i,
           cdef.constructors.count);
    EXIT;
  }
  constructor_t constructor = cdef.constructors.items[i];
  char name[1024] = {0};
  sprintf(name, "%s_%ld", cdef.name, i);
  LLVMTypeRef ftype = ftype_from_constructor(constructor, cdef);
  LLVMValueRef fptr = LLVMAddFunction(gen->module, name, ftype);
  return fptr;
}

LLVMValueRef fptr_from_constructor(constructor_t constructor,
                                   class_entry_t cdef, size_t i) {
  (void)constructor;
  char name[1024] = {0};
  sprintf(name, "%s_%ld", cdef.name, i);
  LLVMValueRef fptr = LLVMGetNamedFunction(gen->module, name);
  if (fptr == NULL) {
    fptr = declare_constructor(cdef, i);
  }
  return fptr;
}

void declare_constructors(class_entry_t cdef) {
  for (size_t i = 0; i < cdef.constructors.count; i++) {
    declare_constructor(cdef, i);
  }
}

LLVMTypeRef ftype_from_method(method_t method, class_entry_t cdef) {
  ltypes ts = {0};
  type_t self = get_type_from_name(cdef.name);
  da_append(&ts, type_to_llvm(get_ptr_of(self)));
  for (size_t j = 0; j < method.arg_names.count; j++) {
    type_t t = method.arg_types.items[j];
    t = get_type_used_in_class(cdef, t);
    if (t.name == NULL) {
      t = get_type_from_name(cdef.name);
    }
    da_append(&ts, type_to_llvm(t));
  }
  type_t ret_type = get_type_used_in_class(cdef, method.return_type);
  LLVMTypeRef ftype =
      LLVMFunctionType(type_to_llvm(ret_type), ts.items, ts.count, 0);
  da_free(ts);
  return ftype;
}

LLVMValueRef declare_method(class_entry_t cdef, method_t method) {
  LLVMTypeRef ftype = ftype_from_method(method, cdef);
  char method_name[1024] = {0};
  sprintf(method_name, "%s_%s", cdef.name, method.name);
  return LLVMAddFunction(gen->module, method_name, ftype);
}

LLVMValueRef fptr_from_method(method_t method, class_entry_t cdef) {
  char method_name[1024] = {0};
  sprintf(method_name, "%s_%s", cdef.name, method.name);
  LLVMValueRef fptr = LLVMGetNamedFunction(gen->module, method_name);
  if (fptr == NULL) {
    fptr = declare_method(cdef, method);
  }
  return fptr;
}

void declare_methods(class_entry_t cdef) {
  for (size_t i = 0; i < cdef.methods.count; i++) {
    method_t method = cdef.methods.items[i];
    declare_method(cdef, method);
  }
}

void push_method_params(method_t method, LLVMValueRef fptr) {
  for (size_t j = 0; j < method.arg_names.count; j++) {
    type_t t = method.arg_types.items[j];
    LLVMValueRef param = LLVMGetParam(fptr, j + 1);
    char *name = strdup(method.arg_names.items[j]);
    LLVMSetValueName(param, name);
    LLVMValueRef ptr = LLVMBuildAlloca(gen->builder, type_to_llvm(t), "ptr");
    LLVMBuildStore(gen->builder, param, ptr);
    named_value_entry_t entry = {name, t, ptr};
    add_named_value(entry);
  }
}

void generate_method(method_t method, class_entry_t cdef, ast_t *m) {
  // LLVMTypeRef ftype = ftype_from_method(method, cdef);
  printf("Generating method %s of class %s\n", method.name, cdef.name);
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

  gen->last_bb = bb_entry;
  LLVMValueRef self = LLVMGetParam(fptr, 0);
  named_value_entry_t self_entry = {
      strdup("self"),
      get_ptr_of(get_type_from_name(cdef.name)),
      self,
  };
  int scope = get_named_values_scope(1);
  gen->current_function_scope = scope;
  add_named_value(self_entry);
  push_method_params(method, fptr);
  generate_compound_for_fun(m->as.method.fdef->as.fundef.body);
  reset_named_values_to_scope(scope);

  if (LLVMGetBasicBlockTerminator(gen->last_bb) == NULL) {
    if (method.return_type.name &&
        strcmp(method.return_type.name, "void") == 0) {
      LLVMBuildRetVoid(gen->builder);
    } else {
      LLVMBuildUnreachable(gen->builder);
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
    LLVMValueRef ptr = LLVMBuildAlloca(gen->builder, type_to_llvm(t), "ptr");
    LLVMBuildStore(gen->builder, param, ptr);
    named_value_entry_t entry = {name, t, ptr};
    add_named_value(entry);
  }
}

void generate_constructor(constructor_t c, class_entry_t cdef, int index,
                          ast_t *body) {
  printf("Generating constructor %d for class %s\n", index, cdef.name);
  // LLVMTypeRef ftype = ftype_from_constructor(c, cdef);
  LLVMValueRef fptr = fptr_from_constructor(c, cdef, index);

  char name[256] = {0};
  sprintf(name, "%s_%d", cdef.name, index);
  strings names = {0};
  types types = {0};
  da_append(&names, strdup("self"));
  type_t self_type = get_ptr_of(get_type_from_name(cdef.name));
  da_append(&types, self_type);
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
  gen->last_bb = bb_entry;
  LLVMValueRef self = LLVMGetParam(fptr, 0);
  LLVMSetValueName(self, "self");
  named_value_entry_t self_entry = {
      strdup("self"),
      get_ptr_of(get_type_from_name(cdef.name)),
      self,
  };
  int scope = get_named_values_scope(0);
  gen->current_function_scope = scope;
  add_named_value(self_entry);
  push_constructor_params(c, fptr);
  generate_compound(body);
  reset_named_values_to_scope(scope);
  if (LLVMGetBasicBlockTerminator(gen->last_bb) == NULL) {
    LLVMBuildRetVoid(gen->builder);
  }
}

void generate_classdef_for_include(ast_t *classdef) {
  if (classdef->as.clazz.temp != NULL) {
    template_t temp = {
        sv_to_cstr(classdef->as.clazz.name.lexeme),
        classdef,
    };
    da_append(&gen->templates, temp);
  } else {
    class_entry_t cdef = entry_from_cdef(classdef->as.clazz);
    add_class(cdef);
    declare_constructors(cdef);
    declare_methods(cdef);
  }
}

class_entry_t temp_entry(ast_t *class_ast) {
  ast_class_t cdef = class_ast->as.clazz;
  members members = {0};
  strings interfaces = {0};
  strings interfaces_names = {0};
  ast_t *ast = NULL;

  char *name = sv_to_cstr(cdef.name.lexeme);

  bool templated = false;

  ptrs to_remove = {0};

  ast_template_t temp = cdef.temp->as.temp;

  for (size_t i = 0; i < cdef.field_count; i++) {
    ast_t *field = cdef.fields[i];
    if (field->kind != AST_MEMBER) {
      continue;
    }
    ast_member_t m = field->as.member;
    specifier_t spec =
        sv_eq(m.specifier.lexeme, SV("public")) ? PUBLIC : PRIVATE;
    type_t type = get_type_from_ast(m.var->as.vardef.type);
    ast_t *init = m.var->as.vardef.value;
    char *name = sv_to_cstr(m.var->as.vardef.name.lexeme);
    member_t entry = {name, spec, type, init};
    da_append(&members, entry);
  }

  class_entry_t c_entry = {
      name, {0}, {0}, members, templated, interfaces, interfaces_names, ast,
  };
  return c_entry;
}

type_t add_class_templated(ast_t *classdef) {
  // class_entry_t cdef = temp_entry(classdef);
  class_entry_t cdef = entry_from_cdef(classdef->as.clazz);

  types ts = {0};
  for (size_t i = 0; i < cdef.interfaces_names.count; ++i) {
    type_t t = get_type_from_name(cdef.interfaces_names.items[i]);
    t = sanitize_type(t);
    da_append(&ts, t);
  }
  int inst_id = get_class_instance_index(cdef.name, ts);

  if (inst_id < 0) {
    inst_id = 0;
    for (size_t i = 0; i < gen->inst_classes.count; ++i) {
      if (strcmp(cdef.name, gen->inst_classes.items[i].class_name) == 0) {
        inst_id++;
      }
    }
  }

  char new_name[256] = {0};

  sprintf(new_name, "%sZ%d", cdef.name, inst_id);

  if (does_type_exist(new_name)) {
    return get_type_from_name(new_name);
  }

  cdef.name = strdup(new_name);
  type_t class_type = t_from_cdef(cdef);
  add_class(cdef);
  add_type(class_type);
  printf("HERE %s\n", new_name);
  return class_type;
}

void generate_classdef(ast_t *classdef) {
  class_entry_t cdef = entry_from_cdef(classdef->as.clazz);
  if (cdef.is_templated) {
    types ts = {0};
    for (size_t i = 0; i < cdef.interfaces_names.count; ++i) {
      type_t t = get_type_from_name(cdef.interfaces_names.items[i]);
      t = sanitize_type(t);
      da_append(&ts, t);
    }
    int inst_id = get_class_instance_index(cdef.name, ts);
    da_free(ts);
    char new_name[128] = {0};
    sprintf(new_name, "%sZ%d", cdef.name, inst_id);
    cdef.name = strdup(new_name);
    declare_constructors(cdef);
    declare_methods(cdef);
  }

  int c_count = 0;
  int m_count = 0;

  ast_class_t clazz = classdef->as.clazz;

  for (size_t i = 0; i < clazz.field_count; ++i) {
    // try to geenrate types present in those fields
    ast_t *field = clazz.fields[i];
    if (field->kind != AST_METHOD) {
      ast_member_t v = field->as.member;
      type_t t = get_type_from_ast(v.var->as.vardef.type);
      (void)t;
    } else {
      if (is_ast_constructor(field)) {
        int index = c_count++;
        constructor_t c = cdef.constructors.items[index];
        for (size_t j = 0; j < c.arg_types.count; ++j) {
          type_t t = sanitize_type(c.arg_types.items[j]);
          (void)t;
          // don't do anything with t but if there is some untemplated types,
          // it will (supposedly) generate them
        }
        // TODO: do it for the body as well
      } else {
        method_t m = cdef.methods.items[m_count++];
        (void)sanitize_type(m.return_type);
        for (size_t j = 0; j < m.arg_types.count; ++j) {
          type_t t = sanitize_type(m.arg_types.items[j]);
          (void)t;
          // don't do anything with t but if there is some untemplated types,
          // it will (supposedly) generate them
        }
      }
    }
  }

  int method_count = 0;
  int constructor_count = 0;

  // saving basic block states etc...

  LLVMBasicBlockRef last_bb = gen->last_bb;
  LLVMValueRef current_ptr = gen->current_ptr;
  int is_new = gen->is_new;
  int current_function_scope = gen->current_function_scope;
  function_entry_t *current_function = gen->current_function;

  gen->current_function = NULL;
  gen->current_ptr = NULL;
  gen->is_new = false;
  gen->current_function_scope = 0;
  gen->last_bb = NULL;

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
  gen->current_function = current_function;
  gen->current_ptr = current_ptr;
  gen->is_new = is_new;
  gen->current_function_scope = current_function_scope;
  gen->last_bb = last_bb;
  LLVMPositionBuilderAtEnd(gen->builder, last_bb);
}

type_t dereference_type(type_t t) {
  if (t.kind != PTR) {
    printf("Cannot dereference non-pointer type\n");
    EXIT;
  }
  return *t.pointed_by;
}

LLVMValueRef get_lm_pointer(ast_t *lm) {
  if (lm->kind == AST_IDENTIFIER) {
    char *name = sv_to_cstr(lm->as.identifier.tok.lexeme);
    int index = get_named_value(name);
    if (index < 0) {
      printf("Identifier '%s' not declared in the current scope 1 (%d)\n", name,
             index);
      EXIT;
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
        EXIT;
      }
      class_entry_t cdef = get_class_by_name(base_type.name);
      if (lm->as.binop.rhs->kind != AST_IDENTIFIER) {
        printf("Malformed access expression: ");
        dump_ast(lm);
        printf("\n");
        EXIT;
      }
      char *field_name = sv_to_cstr(lm->as.binop.rhs->as.identifier.tok.lexeme);
      int index = get_index_of_field(field_name, cdef);
      LLVMValueRef res =
          LLVMBuildStructGEP2(gen->builder, base_type.type, base, index, "");
      free(field_name);
      return res;
    }
    type_t lt = t_of_expr(lm->as.binop.lhs);
    if (lt.kind == CLASS) {
      class_entry_t cdef = get_class_by_name(lt.name);
      int index = get_binop_method_index(lm->as.binop.op.kind, cdef);
      if (index < 0) {
        printf("No method for '" SF "' binop in class %s\n",
               SA(lm->as.binop.op.lexeme), lt.name);
        EXIT;
      }
      LLVMValueRef current_ptr = gen->current_ptr;
      gen->current_ptr = NULL;
      method_t m = cdef.methods.items[index];
      int is_new = gen->is_new;
      gen->is_new = 0;
      LLVMValueRef left = get_lm_pointer(lm->as.binop.lhs);
      LLVMValueRef right = generate_expression(lm->as.binop.rhs);
      type_t rt = t_of_expr(lm->as.binop.rhs);
      right = generate_cast_no_check(right, rt, lt);
      gen->is_new = is_new;
      LLVMValueRef fptr = fptr_from_method(m, cdef);
      LLVMTypeRef ftype = ftype_from_method(m, cdef);
      LLVMValueRef args[] = {left, right};
      LLVMValueRef expr =
          LLVMBuildCall2(gen->builder, ftype, fptr, args, 2, "");
      LLVMValueRef ptr = LLVMBuildAlloca(gen->builder, m.return_type.type, "");
      LLVMBuildStore(gen->builder, expr, ptr);
      if (!gen->is_new) {
        defer_elem_t entry = {get_named_values_scope(1), lt, ptr};
        // da_append(&gen->defers, entry);
        add_defer(entry);
      }
      gen->current_ptr = current_ptr;
      return ptr;
    }
  }
  if (lm->kind == AST_AS_DIR) {
    int is_new = gen->is_new;
    gen->is_new = 0;
    type_t to_cast = t_of_expr(lm);
    LLVMValueRef old_ptr = gen->current_ptr;
    LLVMValueRef ptr = LLVMBuildAlloca(gen->builder, to_cast.type, "");
    gen->current_ptr = ptr;
    LLVMValueRef expr = generate_expression(lm->as.as_dir.expr);
    type_t t = t_of_expr(lm->as.as_dir.expr);
    expr = generate_cast_no_check(expr, t, to_cast);
    gen->current_ptr = old_ptr;
    LLVMBuildStore(gen->builder, expr, ptr);
    gen->is_new = is_new;
    return ptr;
  }
  if (lm->kind == AST_INDEX) {
    ast_t *lhs = lm->as.index.subscripted;
    ast_t *rhs = lm->as.index.index;
    type_t base = t_of_expr(lhs);
    type_t actual = dereference_type(base);
    LLVMValueRef lhs_ptr = generate_expression(lhs);
    LLVMValueRef rhs_val = generate_expression(rhs);
    LLVMValueRef res =
        LLVMBuildGEP2(gen->builder, actual.type, lhs_ptr, &rhs_val, 1, "");
    return res;
  }
  if (lm->kind == AST_NEW_DIR) {
    type_t to_cast = t_of_expr(lm);
    int is_new = gen->is_new;
    LLVMValueRef expr = generate_expression(lm->as.as_dir.expr);
    gen->is_new = 1;
    expr = generate_cast_no_check(expr, to_cast, to_cast);
    LLVMValueRef ptr = LLVMBuildAlloca(gen->builder, type_to_llvm(to_cast), "");
    LLVMBuildStore(gen->builder, expr, ptr);
    gen->is_new = is_new;
    return ptr;
  }
  if (lm->kind == AST_FUNCALL) {
    type_t t = t_of_expr(lm);
    LLVMValueRef value = generate_funcall(lm);
    LLVMValueRef ptr = gen->current_ptr;
    // if (gen->current_ptr == NULL) {
    ptr = LLVMBuildAlloca(gen->builder, type_to_llvm(t), "");
    // if (!gen->is_new) {
    // defer_elem_t elem = {get_named_values_scope(0), t, ptr};
    // add_defer(elem);
    // }
    // }
    LLVMBuildStore(gen->builder, value, ptr);

    return ptr;
  }

  printf("%s:%d TODO: get_lm_pointer %d\n", __FILE__, __LINE__, lm->kind);
  EXIT;
}

void update_value(ast_t *lhs, LLVMValueRef rhs, type_t rt) {
  LLVMValueRef ptr = get_lm_pointer(lhs);
  type_t target_type = t_of_expr(lhs);
  if (target_type.type != LLVMTypeOf(rhs)) {
    rhs = generate_cast(rhs, rt, target_type);
  }
  LLVMBuildStore(gen->builder, rhs, ptr);
}

void print_types(void) {
  printf("TYPES-------------\n");
  for (size_t i = 0; i < gen->types.count; ++i) {
    printf("T %ld: ", i);
    print_type(gen->types.items[i]);
    printf("\n");
  }
  printf("------------------\n");
}

void generate_assign(ast_t *assign) {
  ast_t *lm = assign->as.assign.lhs;
  ast_t *rm = assign->as.assign.rhs;
  LLVMValueRef rhs = generate_expression(rm);
  type_t rt = t_of_expr(rm);
  update_value(lm, rhs, rt);
}

bool is_integer_type(type_t t) {
  if (t.kind != BUILTIN)
    return false;
  return strcmp(t.name, "void") != 0;
}

char *get_real_class_name(const char *n) {
  size_t l = strlen(n);
  int i = -1;
  for (i = l - 1; i >= 0; --i) {
    if (n[i] == 'Z') {
      break;
    }
  }
  return strndup(n, i);
}

type_t get_type_used_in_class(class_entry_t cdef, type_t t) {
  if (t.kind == PTR) {
    return get_ptr_of(get_type_used_in_class(cdef, *t.pointed_by));
  }
  if (!cdef.is_templated || t.kind != TEMPLATED) {
    return sanitize_type(t);
  }
  int arg_index = -1;
  for (size_t j = 0; j < cdef.interfaces_names.count; ++j) {
    char *name = cdef.interfaces_names.items[j];
    if (strcmp(name, t.name) == 0) {
      arg_index = j;
      break;
    }
  }
  // We are at the right index
  // We look for this instance entry
  char *real_name = get_real_class_name(cdef.name);
  char *end;
  const char *numptr = cdef.name + strlen(real_name) + 1;
  int instance_index = strtol(numptr, &end, 10);

  if (instance_index >= (int)gen->inst_classes.count || end == numptr) {
    return t;
  }
  int current_instance = -1;
  for (int i = 0; i < (int)gen->inst_classes.count; ++i) {
    if (strcmp(real_name, gen->inst_classes.items[i].class_name) == 0) {
      current_instance++;
      if (current_instance == instance_index) {
        return sanitize_type(gen->inst_classes.items[i].ts.items[arg_index]);
      }
    }
  }
  printf("Internal error: class %s is not yed instanciated !\n", cdef.name);
  EXIT;
  // inst_templ_class_t instance = gen->inst_classes.items[instance_index];
}

int get_constructor_with_single_arg_matching_type(class_entry_t cdef,
                                                  LLVMTypeRef t) {
  for (size_t i = 0; i < cdef.constructors.count; i++) {
    constructor_t c = cdef.constructors.items[i];
    if (c.arg_names.count != 1)
      continue;
    type_t ct = get_type_used_in_class(cdef, c.arg_types.items[0]);
    if (type_to_llvm(ct) == t) {
      return i;
    }
  }
  return -1;
}

bool are_types_equal(type_t a, type_t b) {
  a = sanitize_type(a);
  b = sanitize_type(b);
  if (a.kind != b.kind)
    return false;
  if (a.kind == PTR) {
    return are_types_equal(dereference_type(a), dereference_type(b));
  }
  if (a.kind == BUILTIN) {
    return LLVMSizeOf(a.type) == LLVMSizeOf(b.type);
  }
  // TODO: add instantiated class type entry maybe ?
  if (a.kind == CLASS || a.kind == TEMPLATED) {
    return strcmp(a.name, b.name) == 0;
  }
  return false;
}

LLVMValueRef generate_cast_no_check(LLVMValueRef value, type_t original_type,
                                    type_t target_type) {
  if (original_type.kind == CLASS && target_type.kind == BUILTIN) {
    printf("Cannot cast class type ");
    print_type(original_type);
    printf(" to builtin type ");
    print_type(target_type);
    printf("\n");
    EXIT;
  }
  target_type = sanitize_type(target_type);
  original_type = sanitize_type(original_type);

  printf("TRYING TO CAST ");
  print_type(original_type);
  printf(" TO ");
  print_type(target_type);
  printf("\n");

  LLVMTypeRef llvm_target_type = type_to_llvm(target_type);
  // LLVMTypeRef LLVMTypeOf(value) = type_to_llvm(original_type);
  // if (LLVMGetTypeKind(LLVMTypeOf(value)) == LLVMPointerTypeKind &&
  //     target_type.kind == PTR) {
  if (original_type.kind == PTR && target_type.kind == PTR) {
    return value;
  }

  if (LLVMGetTypeKind(LLVMTypeOf(value)) == LLVMFloatTypeKind &&
      strcmp(target_type.name, "void") && strcmp(target_type.name, "float")) {
    // float -> int
    return LLVMBuildFPToSI(gen->builder, value, target_type.type, "");
  }

  if (target_type.type == get_type_from_name("bool").type) {
    return LLVMBuildICmp(gen->builder, LLVMIntNE, value,
                         LLVMConstInt(LLVMTypeOf(value), 0, 0), "");
  }

  if (target_type.kind == PTR) {
    value = LLVMBuildIntToPtr(gen->builder, value, llvm_target_type, "");
  } else if (is_integer_type(target_type)) {
    if (LLVMGetTypeKind(LLVMTypeOf(value)) == LLVMPointerTypeKind) {
      value = LLVMBuildPtrToInt(gen->builder, value,
                                get_type_from_name("int").type, "");
    }
    value = LLVMBuildIntCast2(gen->builder, value, llvm_target_type, 1, "");
    printf("IS THIS THE CULPRIT ????\n");
    fflush(stdout);
    LLVMDumpValue(value);
    fflush(stdout);
    printf("???\n");
  } else {
    if (target_type.kind == CLASS) {
      // Look for a single argument constructor that matches values's type.
      // If found, call it.
      class_entry_t cdef = get_class_by_name(target_type.name);
      int constructor_index = get_constructor_with_single_arg_matching_type(
          cdef, LLVMTypeOf(value));
      if (constructor_index < 0) {
        if (are_types_equal(original_type, target_type) &&
            target_type.kind == CLASS) {
          return value;
        } else {
          printf("Cannot convert type ");
          print_type(original_type);
          printf(" to type ");
          print_type(target_type);
          printf(".\n");
        }
        EXIT;
      }
      constructor_t c = cdef.constructors.items[constructor_index];
      LLVMValueRef ptr;
      if (gen->current_ptr == NULL) {
        ptr = LLVMBuildAlloca(gen->builder, target_type.type, "");
      } else {
        ptr = gen->current_ptr;
      }
      if (!gen->is_new) {
        defer_elem_t elem = {get_named_values_scope(1), target_type, ptr};
        add_defer(elem);
      }
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

LLVMValueRef generate_cast(LLVMValueRef value, type_t original_type,
                           type_t target_type) {
  LLVMTypeRef llvm_target_type = type_to_llvm(target_type);
  if (LLVMTypeOf(value) == llvm_target_type) {
    return value;
  }
  return generate_cast_no_check(value, original_type, target_type);
}

void generate_return(ast_t *ret) {
  if (gen->current_function == NULL) {
    printf("Error: Return statement outside of a function.\n");
    EXIT;
  }
  generate_defers(gen->current_function_scope);
  type_t target_type = gen->current_function->return_type;
  LLVMTypeRef llvm_target_type = type_to_llvm(target_type);
  if (ret->as.return_stmt.expr == NULL) {
    if (llvm_target_type != get_type_from_name("void").type) {
      printf("Cannot return void from a function that returns ");
      fflush(stdout);
      LLVMDumpType(llvm_target_type);
      fflush(stdout);
      printf(".\n");
      EXIT;
    }
    LLVMBuildRetVoid(gen->builder);
    return;
  }
  LLVMValueRef value = generate_expression(ret->as.return_stmt.expr);
  type_t orig = t_of_expr(ret->as.return_stmt.expr);
  if (LLVMTypeOf(value) != llvm_target_type) {
    value = generate_cast(value, orig, target_type);
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
  class_entry_t c = get_class_by_name(t.name);
  int default_constructor_index = get_default_constructor(c);
  if (default_constructor_index < 0) {
    printf("Error: No default constructor found for class %s.\n", c.name);
    EXIT;
  }
  LLVMValueRef ptr;
  constructor_t cons = c.constructors.items[default_constructor_index];
  ptr = gen->current_ptr;
  LLVMValueRef fptr = fptr_from_constructor(cons, c, default_constructor_index);
  LLVMTypeRef ftype = ftype_from_constructor(cons, c);
  LLVMBuildCall2(gen->builder, ftype, fptr, &ptr, 1, "");
  defer_elem_t to_defer = {get_named_values_scope(1), t, ptr};
  add_defer(to_defer);
  return NULL;
}

void generate_vardef(ast_t *vardef) {
  type_t type = get_type_from_ast(vardef->as.vardef.type);

  LLVMTypeRef llvm_type = type_to_llvm(type);
  if (llvm_type == NULL) {
    printf("LLVM TYPE IS NULL !\n");
    EXIT;
  }
  LLVMValueRef current_ptr = gen->current_ptr;
  LLVMValueRef ptr = LLVMBuildAlloca(gen->builder, llvm_type, "var");
  gen->current_ptr = ptr;
  LLVMValueRef expr;
  if (vardef->as.vardef.value == NULL) {
    if (type.kind == CLASS) {
      generate_default_value_for_type(type);
    } else {
      expr = generate_default_value_for_type(type);
    }
  } else {
    expr = generate_expression(vardef->as.vardef.value);
    type_t expr_t = t_of_expr(vardef->as.vardef.value);
    if (!are_types_equal(expr_t, type)) {
      expr = generate_cast(expr, expr_t, type);
    }
    char *name = sv_to_cstr(vardef->as.vardef.name.lexeme);

    LLVMBuildStore(gen->builder, expr, ptr);
    LLVMSetValueName(expr, name);
    free(name);
  }
  char *name = sv_to_cstr(vardef->as.vardef.name.lexeme);

  named_value_entry_t entry = {
      name,
      type,
      ptr,
  };
  add_named_value(entry);
  gen->current_ptr = current_ptr;
}

void generate_ct_cte(ast_t *ct_cte) {
  (void)ct_cte;
  printf("%s:%d TODO: generate_ct_cte\n", __FILE__, __LINE__);
  EXIT;
}

bool does_type_exist(const char *name) {
  for (size_t i = 0; i < gen->types.count; i++) {
    if (gen->types.items[i].name == NULL) {
      continue;
    }
    if (strcmp(gen->types.items[i].name, name) == 0) {
      return true;
    }
  }
  return false;
}

int does_method_exist(class_entry_t c, char *name) {
  for (size_t i = 0; i < c.methods.count; i++) {
    if (strcmp(c.methods.items[i].name, name) == 0) {
      return i;
    }
  }
  return -1;
}

class_entry_t entry_from_cdef(ast_class_t cdef) {

  methods methods = {0};
  constructors constructors = {0};
  members members = {0};
  strings interfaces = {0};
  strings interfaces_names = {0};
  ast_t *ast = NULL;

  char *name = sv_to_cstr(cdef.name.lexeme);

  bool templated = false;

  ptrs to_remove = {0};

  if (cdef.temp != NULL) {
    templated = true;
    ast = malloc(sizeof(ast_t));
    ast->kind = AST_CLASS;
    ast->as.clazz = cdef;
    ast_template_t temp = cdef.temp->as.temp;
    for (size_t i = 0; i < temp.count; ++i) {
      ast_tempelem_t t = temp.tempelems[i]->as.tempelem;
      char *type_name = sv_to_cstr(t.type_iden->as.identifier.tok.lexeme);
      char *int_name = sv_to_cstr(t.interface->as.identifier.tok.lexeme);
      da_append(&interfaces, strdup(int_name));
      da_append(&interfaces_names, strdup(type_name));
      printf("Adding templated type\n");
      void *marker = malloc(1);
      da_append(&to_remove, marker);
      type_t temp_type = {strdup(type_name), TEMPLATED, NULL, marker,
                          strdup(int_name),  NULL};
      add_type(temp_type);

      free(type_name);
      free(int_name);
    }
  }

  for (size_t i = 0; i < cdef.field_count; i++) {
    ast_t *field = cdef.fields[i];
    if (field->kind != AST_MEMBER) {
      continue;
    }
    ast_member_t m = field->as.member;
    specifier_t spec =
        sv_eq(m.specifier.lexeme, SV("public")) ? PUBLIC : PRIVATE;
    type_t type = get_type_from_ast(m.var->as.vardef.type);
    ast_t *init = m.var->as.vardef.value;
    char *name = sv_to_cstr(m.var->as.vardef.name.lexeme);
    member_t entry = {name, spec, type, init};
    da_append(&members, entry);
  }

  if (!does_type_exist(name)) {
    class_entry_t c_entry = {
        name,      methods,    constructors,     members,
        templated, interfaces, interfaces_names, ast,
    };
    if (!templated) {
      add_type(t_from_cdef(c_entry));
    }
  }

  for (size_t i = 0; i < cdef.field_count; i++) {
    ast_t *field = cdef.fields[i];
    if (field->kind != AST_METHOD)
      continue;
    if (sv_eq(cdef.name.lexeme, field->as.method.fdef->as.fundef.name.lexeme)) {
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
  }
  class_entry_t entry = {name,      methods,    constructors,     members,
                         templated, interfaces, interfaces_names, ast};

  // remove templated types added
  if (templated) {
    for (size_t i = 0; i < to_remove.count; ++i) {
      for (int j = gen->types.count - 1; j >= 0; --j) {
        type_t t = gen->types.items[j];
        if (t.pointed_by == to_remove.items[i]) {
          free(to_remove.items[j]);
          da_remove(&gen->types, j);
          break;
        }
      }
    }
    da_free(to_remove);
  }

  return entry;
}
