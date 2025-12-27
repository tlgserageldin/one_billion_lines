#include "q_strings.h"
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
there should be a lock on the main array of entries
each thread will be given a chunk of the file that ends on a newline
each thread will then process it's chunk updating a local linked list of
entries. after processing it's chunk, it merges it's local list against the
master list if encounter errors, will return early with error number

the entries are going to be stored in a singly linked list inside of the thread
process
*/

typedef struct {
  float sum;
  float min;
  float max;
  str name;
} entry_t;

entry_t *create_entry(void) {
  entry_t *e = malloc(sizeof(entry_t));
  e->max = 0.0;
  e->min = 0.0;
  e->sum = 0.0;
  e->name.data = NULL;
  e->name.len = 0;
  return e;
}

// allocates an entry on the heap
// fills out the parameters
entry_t *build_entry_from_line(str line) {
  entry_t *e = create_entry();
  snip s = {0};
  s.tail = line;
  s = cut(s.tail, ';');
  memcpy(e->name.data, s.head.data, s.head.len);
  e->name.len = s.head.len;
}

void *process_chunk(void *arg) {
  if (!arg) {
    pthread_exit(NULL);
  }
  str chunk = *(str *)arg;
  snip s = {0};
  s.tail = chunk;
  while (s.tail.len) {
    s = cut(s.tail, '\n');
    str line = s.head;

    entry_t *t = build_entry_from_line(line);
  }
}

int main(void) {
  FILE *f = fopen("/Users/tariqs/Documents/projects/learning/learning_c/"
                  "one_billion_lines/measurements.txt",
                  "r");
  if (!f) {
    return EXIT_FAILURE;
  }

  // generate threads
  const ptrdiff_t n_processors = sysconf(_SC_NPROCESSORS_ONLN);
  pthread_t threads[n_processors];

  for (ptrdiff_t i = 0; i < n_processors; i++) {
    pthread_create(threads[i], NULL, process_chunk, chunk);
  }
}
