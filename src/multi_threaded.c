#include "q_strings.h"
#include "hash_table.h"
//#include <cstdlib>
#include <cstdlib>
#include <pthread.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
  basic architecture should be:
  - distributor
  - creates the chunks for the workers to process
  - process
  - each thread then process their chunk of the file
  - after building their hashtable, a passed pointer should be updated to point to the table they built
  - merge
  - after all threads return merge the hash tables
*/

/*
  A Pthreads thread begins by calling some function that you provide. This
  “thread function” should expect a single argument of type void *, and should
  return a value of the same type.
*/

typedef struct {
    bool ok;
    str *result;
    size_t elements;    
} dist_err;

void *thread_function(void *arg) {
    ptrdiff_t name = *(ptrdiff_t *)arg;
    fprintf(stdout, "My name is %td\n", name);
    return arg;
}

/*
  will return an array of equally sized substrings of input
*/
dist_err distribute(ptrdiff_t n, str input) {
    if (!is_valid_str(input)) {
        return (dist_err){0};
    }

    /*
      signed integer division rounds towards 0
      n items into x groups
      will be left with n % x elements
      so for the first n % n elements we add 1 to the length
      for i = 1..n
      ptrdiff_t segment_size = (input.len / n) + (i <= n mod x ? 1 : 0)
      example:
      11 / 3 = 3
      11 % 3 = 2

      stuff[0].len = 11/3 + 1, because i+1 = 1 <= 11 % 3
      stuff[1].len = 11/3 + 1, because i+1 = 2 <= 11 % 3
      stuff[2].len = 11/3 + 0, because i+1 = 3 !<= 11 % 3
    */

    str a[n];
    ptrdiff_t total_length = 0;    
    for (ptrdiff_t i = 0; i < n; ++i) {
      a[i].len = (input.len / n) + ((i + 1) <= input.len % n ? 1 : 0);
      // a[i].data = calloc((size_t)a[i].len, sizeof(char));
      a[i].data = NULL;
      total_length += a[i].len;
      fprintf(stdout,
              "%td-th string, is of length %td.\n"
              "Amount of space made = %td\n"
              "Expected space: %td\n",
              i, a[i].len, total_length, input.len);
    }
    return (dist_err) {
        .ok = true, .result = a, .elements = n
    };
}

long get_file_length(FILE *f) {
    if (fseek(f, 0L, SEEK_END) != 0) {
        return -1;
    }

    long res = ftell(f);
    if (res == -1) {
        return -1;
    }

    if (fseek(f, 0L, SEEK_SET) != 0) {
        return -1;
    }

    return res;    
}  

int main(void) {
//    FILE *f = fopen("/Users/tariqs/Documents/projects/code/one_billion_lines/data/measurements.txt",
//                    "r");
    FILE *f = fopen("/Users/tariqs/Documents/projects/code/one_billion_lines/data/1000_lines.txt",
                    "r");
    if (!f) {
        return EXIT_FAILURE;
    }

    // generate threads
    //    const ptrdiff_t n_processors = sysconf(_SC_NPROCESSORS_ONLN);
    //    fprintf(stdout, "Making %td threads.\n\n", n_processors);
    //    pthread_t threads[n_processors];
    //    for (ptrdiff_t i = 0; i < n_processors; ++i) {
    //        pthread_create(&threads[i], NULL, thread_function, &i);
    //    }
}
