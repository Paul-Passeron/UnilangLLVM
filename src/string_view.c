/**
 * string_view.c
 * Copyright (C) 2024 Paul Passeron
 * STRING_VIEW source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/string_view.h"
#include "../include/regexp.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void sv_consume(string_view_t *s) {
  if (s->length <= 1)
    return;
  s->contents++;
  s->length--;
}

char *sv_to_cstr(string_view_t s) {
  char *res = malloc(s.length + 1);
  for (size_t i = 0; i < s.length; i++) {
    res[i] = s.contents[i];
  }
  res[s.length] = 0;
  return res;
}

bool sv_matches_exact(string_view_t pattern, string_view_t string,
                      string_view_t *rest) {
  char *pat = sv_to_cstr(pattern);
  char *str = sv_to_cstr(string);
  char *r;
  bool res = matches_exact(pat, str, &r);
  int l = strlen(r);
  *rest = (string_view_t){malloc(l), l};
  memcpy(rest->contents, r, l);
  free(pat);
  free(str);
  return res;
}

int length_until(string_view_t s, char c) {
  for (size_t i = 0; i < s.length; i++) {
    if (s.contents[i] == c)
      return i;
  }
  return -1;
}
