#include "q_strings.h"
#include "hash_table.h"
// #include <cstdlib>
#include <assert.h>
// #include <cstdlib.h>
// #include <cstdlib>
#include <climits>
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
  - after building their hashtable, a passed pointer should be updated to point
    to the table they built
  - merge
  - after all threads return merge the hash tables

*/

/*

  A Pthreads thread begins by calling some function that you provide. This
  “thread function” should expect a single argument of type void *, and should
  return a value of the same type.

*/

// distribute return struct
typedef struct {
    bool ok;
    str *result;
    size_t elements;
} dist_res;

dist_res build_result(bool ok, str *r, size_t e) {
  return (dist_res) {
    .ok = ok, .result = r, .elements = e,
  };    
}  

// placeholder function for threads to call
void *thread_function(void *arg) {
    ptrdiff_t name = *(ptrdiff_t *)arg;
    fprintf(stdout, "My name is %td\n", name);
    return arg;
}

/*

  will return an array of str that point into input
  breaking on \n, of a number of slices <= x

*/
dist_res distribute(ptrdiff_t x, str input) {
  
    if (!is_valid_str(input) || x >= input.len || x <= 0 || *(input.data + input.len) != '\n') {
        return (dist_res){0};
    }

    /*

      signed integer division rounds towards 0
      n items into x groups
      will be left with n % x elements
      so for the first n % x elements we add 1 to the length
      for i = 0..x
      ptrdiff_t segment_size = (n / x) + (i <= n mod x ? 1 : 0)
      example:
      11 / 3 = 3
      11 % 3 = 2

      stuff[0].len = 11/3 + 1, because i+1 = 1 <= 11 % 3
      stuff[1].len = 11/3 + 1, because i+1 = 2 <= 11 % 3
      stuff[2].len = 11/3 + 0, because i+1 = 3 !<= 11 % 3

    */

    /*

      to guarantee that you have <= x slices that break on newline
      precalculate the lengths of each of the slices.

      walk each str from optimal spot to next newline
      if you have atleast the optimal number of chars in each of the
      strs until the final, the final str is guaranteed to have less or
      equal to ( n / x ). you always fit all of the input into <= x slices

     */

    str slices[x];
    memset(&slices, 0, sizeof(str) * x);

    dist_res r = {0};
    unsigned char *head, *tail;
    head = tail = input.data;
    snip s = {0};
    for (int i = 0; i < x; ++i) {

        ptrdiff_t optimal_length =
            (input.len / x) + ((i + 1) <= (input.len % x) ? 1 : 0);
        
        // bounds check
        if (((head + optimal_length) - input.data) > input.len) {
            ptrdiff_t remainder = input.len - (head - input.data);           
            tail = head + remainder;
        } else {
            tail = head + optimal_length;
        }

        if (tail == input.data + input.len) {
            slices[i] = slice(head, tail);
            return build_result(true, slices, i + 1);
        } else if (*tail != '\n') {
          s.tail = (str){.data = tail, .len = input.len - (tail - input.data)};
          s = cut(s.tail, '\n');
          if (!s.ok) {
            return r;
          }            
          tail = s.head.data + s.head.len;          
        }

        slices[i] = slice(head, tail);
        head = tail;
    }

    return build_result(true, slices, x);    
    
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

int main() {
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

    uint64_t file_len = get_file_length(f);
    str input = {0};
    input.data = calloc(file_len, sizeof(char));    
    if (input.data == NULL) {
        return EXIT_FAILURE;
    }
    
    input.len = fread(input.data, sizeof(char), file_len, f);

    dist_res res = distribute(5, input);

    return 0;    
}
