/**
 * regexp.c
 * Copyright (C) 2024 Paul Passeron
 * REGEXP source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/regexp.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void match_single(const char *pattern, const char *string, bool *worked,
                  int *pattern_advanced) {
  *pattern_advanced = 1;
  char to_test = *pattern;
  pattern++;
  if (to_test == '\\') {
    // handle escaping
    switch (*pattern) {
    case '*':
      to_test = '*';
      break;
    case '[':
      to_test = '[';
      break;
    case ']':
      to_test = ']';
      break;
    case ')':
      to_test = ')';
      break;
    case '(':
      to_test = '(';
      break;
    case '\\':
      to_test = '\\';
      break;
    case '?':
      to_test = '?';
      break;
    case 'n':
      to_test = '\n';
      break;
    case 't':
      to_test = '\t';
      break;
    case 'r':
      to_test = '\r';
      break;
    case 'b':
      to_test = '\b';
      break;
    case '0':
      to_test = 0;
      break;
    default:
      break;
    }
    *pattern_advanced = 2;
  }
  *worked = *string == to_test;
}

char get_actualchar(const char *str, int *length) {
  *length = 1;
  if (!*str)
    return 0;
  char ch = *str;
  str++;
  if (ch == '\\') {
    *length = 2;
    switch (*str) {
    case '*':
      ch = '*';
      break;
    case '[':
      ch = '[';
      break;
    case ']':
      ch = ']';
      break;
    case ')':
      ch = ')';
      break;
    case '(':
      ch = '(';
      break;
    case '\\':
      ch = '\\';
      break;
    case '?':
      ch = '?';
      break;
    case 'n':
      ch = '\n';
      break;
    case 't':
      ch = '\t';
      break;
    case 'r':
      ch = '\r';
      break;
    case 'b':
      ch = '\b';
      break;
    case '0':
      ch = 0;
      break;
    default:
      break;
    }
  }
  return ch;
}

void regexp(const char *pattern, const char *string, bool *pattern_finished,
            int *string_matched) {
  const char *src_string = string;
  *pattern_finished = false;
  *string_matched = 0;
  while (*pattern && *string) {
    char c = *pattern;
    if (c == '(') {
      pattern++;
      const char *pat = pattern;
      while (*pattern && *pattern != ')') {
        pattern++;
      }
      char *sub_pat = (malloc(pattern - pat + 1));
      memcpy(sub_pat, pat, pattern - pat + 1);
      sub_pat[pattern - pat] = 0;
      pattern++;
      while (true) {
        bool finished = false;
        int strmatched = 0;
        regexp(sub_pat, string, &finished, &strmatched);
        if (!finished || !*string) {
          break;
        }
        string += strmatched;
        *string_matched += strmatched;
      }
      free(sub_pat);

      continue;
    } else if (c == '?') {
      pattern++;
      string++;
      *string_matched += 1;
    } else if (c == '*') {
      if (!*(pattern + 1)) {
        *pattern_finished = true;
        *string_matched = strlen(src_string);
        return;
      }
      uint32_t string_length = strlen(string);
      for (size_t j = 0; j < string_length; j++) {
        bool finished = false;
        int string_match = 0;
        regexp(pattern + 1, string + j, &finished, &string_match);
        if (finished) {
          *pattern_finished = true;
          *string_matched += string_match + j;
          return;
        }
        if (!*(string + j + 1)) {
          *pattern_finished = false;
          return;
        }
      }
    } else if (c == '[') {
      bool found = false;
      pattern++;
      if (!*pattern || *pattern == ']') {
        exit(1);
      }
      do {
        if (found) {
          pattern++;
          continue;
        }
        int l = 0;
        char start = get_actualchar(pattern, &l);
        pattern += l;
        char delim = *pattern;
        if (!delim) {
          exit(1);
        }
        if (start != delim && start == '-') {
          if (*string == '-') {
            pattern--;
            found = true;
            *string_matched += 1;
            string++;
            continue;
          }
          return;
        }
        if (delim != '-') {
          exit(1);
        }
        pattern++;
        char end = get_actualchar(pattern, &l);
        pattern += l;
        if (!end || end == ']') {
          exit(1);
        }
        char s = *string;
        if (s >= start && s <= end) {
          found = true;
          *string_matched += 1;
          string++;
          if (!*pattern) {
            *pattern_finished = true;
          }
        }
      } while (*pattern && *pattern != ']');
      if (!found) {
        return;
      }
      pattern++;
    } else {
      bool worked;
      int amount;
      match_single(pattern, string, &worked, &amount);
      if (worked) {
        string++;
        *string_matched += 1;
        pattern += amount;
      } else {
        return;
      }
    }
  }
  while (*pattern == '(') {
    while (*pattern && *pattern != ')') {
      pattern++;
    }
    pattern++;
  }
  if (!*pattern) {
    *pattern_finished = true;
  }
}

bool matches_exact(const char *pattern, char *string, char **rest) {
  *rest = string;
  bool res;
  int l;
  regexp(pattern, string, &res, &l);
  if (res) {
    *rest = string + l;
  }
  return res;
}

bool sv_matches_exact(string_view_t pattern, string_view_t string,
                      string_view_t *rest) {
  *rest = string;
  bool res;
  int l;
  sv_regexp(pattern, string, &res, &l);
  if (res) {
    *rest = (string_view_t){string.contents + l, string.length - l};
  }
  return res;
}

void sv_regexp(string_view_t pattern, string_view_t string,
               bool *pattern_finished, int *string_matched) {
  *pattern_finished = false;
  *string_matched = 0;
  size_t pat_index = 0;
  size_t str_index = 0;
  while (pat_index < pattern.length && str_index < string.length) {
    char c = pattern.contents[pat_index];
    if (c == '(') {
      pat_index++;
      size_t start_index = pat_index;
      while (pat_index < pattern.length && pattern.contents[pat_index] != ')') {
        pat_index++;
      }
      string_view_t sub_pat = (string_view_t){
          .contents = pattern.contents + start_index,
          .length = pat_index - start_index,
      };
      pat_index++;
      while (true) {
        bool finished = false;
        int strmatched = 0;
        string_view_t to_match = (string_view_t){
            string.contents + str_index,
            string.length - str_index,
        };
        sv_regexp(sub_pat, to_match, &finished, &strmatched);
        if (!finished || str_index >= string.length) {
          break;
        }
        str_index += strmatched;
        *string_matched += strmatched;
      }
      continue;
    } else if (c == '?') {
      pat_index++;
      str_index++;
      *string_matched += 1;
    } else if (c == '*') {
      if (pat_index + 1 >= pattern.length) {
        *pattern_finished = true;
        *string_matched = string.length;
        return;
      }
      size_t string_length = string.length - str_index;
      for (size_t j = 0; j < string_length; j++) {
        bool finished = false;
        int string_match = 0;
        string_view_t to_match =
            (string_view_t){string.contents + str_index + j, string_length - j};
        string_view_t new_pat = (string_view_t){
            pattern.contents + pat_index + 1, pattern.length - pat_index - 1};
        sv_regexp(new_pat, to_match, &finished, &string_match);
        if (finished) {
          *pattern_finished = true;
          *string_matched += string_match + j;
          return;
        }
        if (str_index + j >= string.length) {
          *pattern_finished = false;
          return;
        }
      }
    } else if (c == '[') {
      bool found = false;
      pat_index++;
      if (pat_index >= pattern.length || pattern.contents[pat_index] == ']') {
        printf("Syntax error in regexp: [] without body\n");
        exit(1);
      }
      do {
        if (found) {
          pat_index++;
          continue;
        }
        int l = 0;
        char start = get_actualchar(pattern.contents + pat_index, &l);
        pat_index += l;
        char delim = pattern.contents[pat_index];
        if (start != delim && start == '-') {
          if (string.contents[str_index] == '-') {
            pat_index--;
            found = true;
            *string_matched += 1;
            str_index++;
            continue;
          }
          // return;
        }
        if (delim != '-') {
          printf("Syntax error in regexp: Expected '-' delimeter in [] "
                 "range.\n");
          exit(1);
        }
        pat_index++;
        if (pat_index >= pattern.length) {
          printf("Syntax error in regexp: [ without ] in range.\n");
          exit(1);
        }
        char end = get_actualchar(pattern.contents + pat_index, &l);

        pat_index += l;
        if (end == ']' || pat_index >= pattern.length) {
          printf("Syntax error in regexp: No right part of range [].\n");
          exit(1);
        }
        char s = string.contents[str_index];
        if (s >= start && s <= end) {
          found = true;
          *string_matched += 1;
          str_index++;
          if (pat_index >= pattern.length) {
            *pattern_finished = true;
          }
        }
      } while (pat_index < pattern.length &&
               pattern.contents[pat_index] != ']');
      if (!found) {
        return;
      }
      pat_index++;
    } else {
      bool worked;
      int ammount;
      match_single(pattern.contents + pat_index, string.contents + str_index,
                   &worked, &ammount);
      if (worked) {
        str_index++;
        *string_matched += 1;
        pat_index += ammount;
      } else {
        return;
      }
    }
  }
  // why ?
  while (pattern.contents[pat_index] == '(') {
    while (pat_index < pattern.length && pattern.contents[pat_index] != ')') {
      pat_index++;
    }
    pat_index++;
  }
  if (pat_index >= pattern.length) {
    *pattern_finished = true;
  }
}

char *unescape_string(const char *input) {
  if (!input)
    return NULL;

  // Allocate enough space for the output string (at most as long as input)
  size_t len = strlen(input);
  char *output = (char *)malloc(len + 1); // +1 for null terminator
  if (!output)
    return NULL;

  size_t out_idx = 0; // Output index

  for (size_t i = 0; i < len; ++i) {
    if (input[i] == '\\' && i + 1 < len) {
      switch (input[i + 1]) {
      case 'n':
        output[out_idx++] = '\n';
        break;
      case 't':
        output[out_idx++] = '\t';
        break;
      case 'r':
        output[out_idx++] = '\r';
        break;
      case 'b':
        output[out_idx++] = '\b';
        break;
      default:
        output[out_idx++] = input[i];
        break;
      }
      ++i; // Skip the next character
    } else {
      output[out_idx++] = input[i];
    }
  }
  output[out_idx] = '\0';
  return output;
}