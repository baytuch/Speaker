
#include <stdlib.h>
#include <string.h>
#include "sleep.h"
#include "utils.h"


char *strcopy(const char *source){
  char *res;
  size_t str_size = strlen(source);
  res = (char*) malloc((str_size + 1) * sizeof(char));
  strncpy(res, source, (str_size + 1) * sizeof(char));
  return res;
}

char *strinit(const unsigned int str_size){
  char *res;
  res = (char*) malloc((size_t) (str_size + 1) * sizeof(char));
  memset(res, 0x00, (size_t) (str_size + 1) * sizeof(char));
  return res;
}

wchar_t *w_strinit(const unsigned int str_size){
  wchar_t *res;
  res = (wchar_t*) malloc((size_t) (str_size + 1) * sizeof(wchar_t));
  memset(res, 0x00, (size_t) (str_size + 1) * sizeof(wchar_t));
  return res;
}

void free_mem(void *ptr){
  free(ptr);
}

void waiter(bool &flag){
  while (flag){
    Sleep(100);
  }
}
