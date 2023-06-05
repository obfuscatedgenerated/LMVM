#ifndef LMVM_CHECKED_MALLOC_H
#define LMVM_CHECKED_MALLOC_H

#include <stdlib.h>

void *checked_malloc(size_t size);

void checked_free(void *ptr);

void silent_checked_free(void *ptr);

void strict_checked_free(void *ptr);

void *checked_realloc(void *ptr, size_t size);

void *checked_calloc(size_t nmemb, size_t size);

#endif //LMVM_CHECKED_MALLOC_H
