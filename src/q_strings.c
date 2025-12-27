#include "q_strings.h"

bool are_equal(str a, str b) {
  if (a.len != b.len) {
    return false;
  } else if (a.data == b.data) {
    return true;
  } else if (!a.data || !b.data) {
    return false;
  }
  return !memcmp(a.data, b.data, a.len);
}

str slice(char *start, char *end) {
  str s = {0};
  s.data = start;
  s.len = start ? end - start : 0;
  return s;
}

snip cut(str s, char c) {
  snip n = {0};
  if (!s.len) {
    return n;
  } // null pointer case

  char *beggining = s.data;
  char *end = s.data + s.len;
  char *cut = beggining;

  for (; cut < end && *cut != c; cut++)
    ;

  n.ok = cut < end;
  n.head = slice(beggining, cut);
  // if ok then we want char after delim,
  // otherwise we hit the end so just return a 0 len str
  n.tail = slice(n.ok ? cut + 1 : cut, end);

  return n;
}

// str is valid if points to something and has > 0 length
inline bool is_valid_str(str a) {
  return a.len && a.data; 
}
