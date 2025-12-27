#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define macro_var(name) concat(name, __LINE__)

#define defer(start, end) for ( \
  int macro_var(_i_) = (start, 0); \
  !macro_var(_i_); \
  (macro_var(_i_) += 1), end) \

#define at_end(end) for ( \
  int macro_var(_i_) = 0; \
  !macro_var(_i_); \
  (macro_var(_i_) += 1), end) \

#define LINE_LEN 1024
#define NAME_MAX 100
#define TEMP_MAX 100
#define MAX_PLACES 1000
#define BUFF_SIZE 16*1024*1024 // 16 MB, L3 cache on M2 is 16 mb
//#define BUFF_SIZE 30

#define TRUE 1
#define FALSE 0

typedef struct entry_t entry_t;
struct entry_t {
  float sum;
  float min;
  float max;
  size_t n_temps;
  char name[NAME_MAX];
};

static inline int entry_cmp(const void *key, const void *element) {
  // want NULL named entries to the right side of array
  return strcmp(((entry_t*)key)->name, ((entry_t*)element)->name);
}

static inline float max(const float a, const float b) { return a > b? a: b; }
static inline float min(const float a, const float b) { return a < b? a: b; }

int main(void) {
  FILE *f = fopen("/Users/tariqs/Documents/projects/learning/learning_c/one_billion_lines/measurements.txt", "r");
  if (!f) {
    perror("Failed to open file.");
    return EXIT_FAILURE;
  }

  char *buff = malloc(BUFF_SIZE);
  if (!buff) {
    perror("Failed to alloc initial buffer.");
    fclose(f);
    return EXIT_FAILURE;
  }

  entry_t places[MAX_PLACES] = {0};
  size_t n_places = 0;

  char *head = buff;
  char *tail = buff;
  size_t remainder = 0;
  size_t bytes_read = 0;
  while ((bytes_read = fread(buff + remainder, 1, BUFF_SIZE - 1 - remainder, f)) > 0) {
    *(buff + bytes_read + remainder) = '\0'; // should always be zero, but in case

    char name[NAME_MAX] = {0};
    size_t name_len = 0;
    char temp[TEMP_MAX] = {0};
    size_t temp_len = 0;
    while (*head != '\0') {
      if (*head == '\n') {
        while (*tail != ';') {
          name[name_len++] = *tail;
          if (name_len == NAME_MAX - 1) { // ensure that there is always room to null-terminate
            perror("tried to parse name with too many chars.");
            free(buff);
            pclose(f);
            return EXIT_FAILURE;
          }
          tail++;
        }
        name[name_len] = '\0';
        tail++;

        while (tail != head) {
          temp[temp_len++] = *tail;
          if (temp_len == TEMP_MAX - 1) { // ensure that there is always room to null-terminate
            perror("tried to parse temp with too many chars.");
            free(buff);
            pclose(f);
            return EXIT_FAILURE;
          }
          tail++;
        }
        temp[temp_len] = '\0';
        float tempf = strtof(temp, NULL);

        entry_t key = {0};
        strlcpy(key.name, name, NAME_MAX);
        entry_t *res = bsearch(&key, places, n_places, sizeof(entry_t), entry_cmp);
        if (res) {
          res->max = max(tempf, res->max);
          res->min = min(tempf, res->min);
          res->sum += tempf;
          res->n_temps++;
        } else {
          places[n_places] = (entry_t) { .max = tempf, .min = tempf, .sum = tempf, .n_temps = 1 };
          strlcpy(places[n_places].name, name, NAME_MAX);
          if (!(n_places == 0 || entry_cmp(&key, &places[n_places]) > 0)) {
            qsort(places, n_places, sizeof(entry_t), entry_cmp);
          }
          n_places++;
        }
        tail = ++head;
        name_len = temp_len = 0;
      } else {
        head += 1;
      }
    }
    if (tail == head - 1) {
      remainder = 0;
      head = buff;
      tail = buff;
    } else {
      remainder = strlen(tail);
      memmove(buff, tail, remainder);
      tail = head = buff;
    }
  }

  if (feof(f)) {
  } else if (ferror(f)) {
    perror("Error reading from file into buff.");
    free(buff);
    fclose(f);
    return EXIT_FAILURE;
  }

  free(buff);
  printf("Results:\n");
  for(size_t i = 0; i < n_places; i++) {
    printf("Name: %s, Mean: %f, Min: %f, Max: %f\n", 
           places[i].name, places[i].sum/places[i].n_temps, 
           places[i].min, places[i].max);
//    free(places[i].place);
  }
  fclose(f);
  return EXIT_SUCCESS;
}
