/*
Start with behavioral contracts your API promises:// {{{
1.	Creation/Destruction
•	ht_create() returns non-NULL.
•	double-destroy is not allowed? (decide; then test accordingly)
2.	Lookup
•	empty GET returns NULL.
•	missing key returns NULL.
•	present key returns exactly the value pointer you stored.
3.	Insert/Overwrite
•	inserting same key twice overwrites previous value (pointer equality).
4.	Growth/Resizing
•	after many inserts (thousands), every inserted key is retrievable.
5.	Key equality
•	two different str instances
  with identical bytes/length compare equal in the table.
•	near-miss keys (same prefix, different length) do not alias.
6.	Collision tolerance (implicit via lots of inserts).
7.	Edge inputs
•	zero-length key if your str type allows it (decide policy, then test).
•	very long keys (e.g., 4–8 KB).
8.	Stability
•	repeated GET on same key returns the same pointer across calls.

Once the basics pass, add:
•	Table-driven tests:
  build an array of {key, value} pairs and loop.
•	Random round-trip:
  generate random ASCII keys, insert N, then re-lookup all.
•	Negative tests:
  passing NULL table pointer should not segfault
  if you choose to check and handle— define your stance and test it).// }}}
*/
#include "hash_table.h"
#include "q_strings.h"
#include "test_helpers.h"
#include "test_runner.h"

#define FN_LIST                                                                \
  X(literal_to_str)                                                            \
  X(create_returns_nonnull)                                                    \
  X(double_destroy)                                                            \
  X(destroy_on_null)                                                           \
  X(empty_search_returns_null)                                                 \
  X(incorrect_key_returns_null)                                                \
  X(correct_key_gets_correct_value)                                            \
  X(setting_twice_updates_value) \
  X(thousands_of_inserts) \

int literal_to_str(void) {
  str a = {
      .data = "new key",
      .len = 7,
  };
  str b = LITERAL_TO_STR("new key");

  CHECK(are_equal(a, b));
  return 0;
}
int create_returns_nonnull(void) {
  ht *t = ht_create();
  CHECK(t);
  ht_destroy(&t);
  return 0;
}
int double_destroy(void) {
  ht *t = ht_create();
  ht_destroy(&t);
  CHECK(ht_destroy(&t));
  return 0;
}
int destroy_on_null(void) {
  ht *t = NULL;
  CHECK(ht_destroy(&t));
  return 0;
}

int empty_search_returns_null(void) {
  str temp = {
      .data = NULL,
      .len = 0,
  };
  CHECK(!ht_search(NULL, temp));
  return 0;
}

int incorrect_key_returns_null(void) {
  ht *t = ht_create();
  str key = {
      .data = "new key",
      .len = strlen(key.data),
  };
  str wrong_key = {
      .data = "wrong key",
      .len = strlen(key.data),
  };
  int val = 1;
  ht_insert(t, key, &val);
  CHECK(ht_search(t, wrong_key) == NULL);
  ht_destroy(&t);
  return 0;
}

int correct_key_gets_correct_value(void) {
  ht *t = ht_create();
  str key = {
      .data = "new key",
      .len = strlen(key.data),
  };
  str wrong_key = {
      .data = "wrong key",
      .len = strlen(key.data),
  };
  int val = 1;
  ht_insert(t, key, &val);
  CHECK(ht_search(t, wrong_key) == NULL);
  CHECK(ht_search(t, key) == &val);
  ht_destroy(&t);
  return 0;
}

int setting_twice_updates_value(void) {
  ht *t = ht_create();
  str key = {
      .data = "new key",
      .len = strlen(key.data),
  };
  int val = 1;
  int new_val = 23;
  ht_insert(t, key, &val);
  ht_insert(t, key, &new_val);
  CHECK(ht_search(t, key) == &new_val);
  ht_destroy(&t);
  return 0;
}

static str gen_key(size_t i) {
  char buffer[32];
  int len = snprintf(buffer, sizeof(buffer), "key_%06zu", i);

  return (str){
    .data = buffer,
    .len = (size_t)len,
  };
}

// generates a bit string masquerading as a mem addr
static void *gen_val(size_t i) {
  return (void *)(uintptr_t)i;;
}

int thousands_of_inserts(void) {
  ht *table = ht_create();
  for (int i = 0; i < 10000; i += 1) {
    str key = gen_key(i);
    void *val = gen_val(i);
    CHECK(ht_insert(table, key, val) == 0);
  }
  ht_destroy(&table);
  return 0;
}

int two_instance_key_equality(void);
int near_miss_keys(void);

int zero_length_keys(void);
int very_long_keys(void);

int consistent_get_returns(void);

int random_round_trip(void);

#define X(token)                                                               \
  (test_case){.result = 0, .name = LITERAL_TO_STR(#token), .fn = token},

test_case tests[] = {FN_LIST};
#undef X

#define FN_COUNT (sizeof(tests) / sizeof(tests[0]))

int main(void) {
  run_tests(tests, FN_COUNT);
  results(tests, FN_COUNT);
  return 0;
};
