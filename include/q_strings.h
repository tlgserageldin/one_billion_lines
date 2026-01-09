#pragma once

#include <stddef.h>
#include <string.h>

#define S(s) ((str){.data = (unsigned char *)s, .len = sizeof(s) - 1})

typedef struct {
  unsigned char *data;
  ptrdiff_t len;
} str;

typedef struct {
  str head;
  str tail;
  bool ok;
} snip;

// returns a substring from start to end
str slice(unsigned char *start, unsigned char *end);

// returns a snip, splitting s on first instance of c
snip cut(str s, char c);

bool are_equal(str a, str b);

bool is_valid_str(str a);
