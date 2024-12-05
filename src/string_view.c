/**
 * string_view.c
 * Copyright (C) 2024 Paul Passeron
 * STRING_VIEW source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/string_view.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sv_consume(string_view_t *s) {
  if (s->length <= 1)
    return;
  s->contents++;
  s->length--;
}

void FREE(void *ptr) {
  printf("%p\n", ptr);
  free(ptr);
}

char *sv_to_cstr(string_view_t s) {
  char *res = calloc(s.length + 1, 1);
  for (size_t i = 0; i < s.length; i++) {
    res[i] = s.contents[i];
  }
  res[s.length] = 0;
  return res;
}
#if 0
bool sv_matches_exact(string_view_t pattern, string_view_t string,
                      string_view_t *rest) {

  char *pat = malloc(pattern.length + 1);
  memcpy(pat, pattern.contents, pattern.length);
  pat[pattern.length] = 0;
  char *str = malloc(string.length + 1);
  memcpy(str, string.contents, string.length);
  str[string.length] = 0;
  char *r;
  bool res = matches_exact(pat, str, &r);
  int l = strlen(r);
  *rest = (string_view_t){string.contents + (r - str), l};
  FREE(pat);
  FREE(str);
  return res;
}
#endif

int length_until(string_view_t s, char c) {
  for (size_t i = 0; i < s.length; i++) {
    if (s.contents[i] == c)
      return i;
  }
  return -1;
}

string_view_t from_file(FILE *f) {
  long current = ftell(f);
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *str = malloc(size + 1);
  fread(str, 1, size, f);
  fseek(f, current, SEEK_SET);
  return (string_view_t){str, size};
}

bool sv_eq(string_view_t a, string_view_t b) {
  if (a.length != b.length)
    return false;
  return !memcmp(a.contents, b.contents, a.length);
}