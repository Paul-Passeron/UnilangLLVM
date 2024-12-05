/**
 * regexp.h
 * Copyright (C) 2024 Paul Passeron
 * REGEXP header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#ifndef REGEXP_H
#define REGEXP_H

#include "string_view.h"
#include <stdbool.h>

void regexp(const char *pattern, const char *string, bool *pattern_finished,
            int *string_matched);

void sv_regexp(string_view_t pattern, string_view_t string,
               bool *pattern_finished, int *string_matched);

bool matches_exact(const char *pattern, char *string, char **rest);

bool sv_matches_exact(string_view_t pattern, string_view_t string,
                      string_view_t *rest);

#endif // REGEXP_H
