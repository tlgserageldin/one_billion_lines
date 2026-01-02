#pragma once

#include <stddef.h>
#include <string.h>

#define LITERAL_TO_STR(s) ((str){.data = (char *)s, .len = sizeof(s) - 1})

#define BAD_STR ((str){.data = NULL, .len = 0})

typedef struct {
  char *data;
  ptrdiff_t len;
} str;

typedef struct {
  str head;
  str tail;
  bool ok;
} snip;

// returns a substring from start to end
str slice(char *start, char *end);

// returns a snip, splitting s on first instance of c
snip cut(str s, char c);

bool are_equal(str a, str b);

bool is_valid_str(str a);
