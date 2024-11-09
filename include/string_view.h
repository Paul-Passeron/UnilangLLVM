/**
 * string_view.h
 * Copyright (C) 2024 Paul Passeron
 * STRING_VIEW header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

// File created by the new_file tool !

#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include <stddef.h>

typedef struct string_view_t {
  char *contents;
  size_t length;
} string_view_t;

#define SV(s)                                                                  \
  (string_view) { (s), sizeof(s) - 1 }

#endif // STRING_VIEW_H
