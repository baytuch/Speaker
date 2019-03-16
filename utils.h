
#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

char *strcopy(const char *source);
char *strinit(const unsigned int str_size);
wchar_t *w_strinit(const unsigned int str_size);
void free_mem(void *ptr);
void waiter(bool &flag);

#endif // UTILS_H_INCLUDED
