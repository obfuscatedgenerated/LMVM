#include "common/checked_alloc.h"

#include <stdlib.h>
#include <stdio.h>

void *checked_malloc(size_t size) {
    void *ptr = malloc(size);

    if (ptr == NULL) {
        fprintf(stderr, "Internal Error: Out of memory, allocation failure\n");
        exit(EXIT_FAILURE);
    }

    return ptr;
}


void checked_free(void *ptr) {
    if (ptr == NULL) {
        puts("Internal Warning: Attempted to free NULL pointer");
        return;
    }

    free(ptr);
}

void silent_checked_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    free(ptr);
}

void strict_checked_free(void *ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "Internal Error: Attempted to free NULL pointer\n");
        exit(EXIT_FAILURE);
    }

    free(ptr);
}


void *checked_realloc(void *ptr, size_t size) {
    void *new_ptr = realloc(ptr, size);

    if (new_ptr == NULL) {
        fprintf(stderr, "Internal Error: Out of memory, allocation failure\n");
        exit(EXIT_FAILURE);
    }

    return new_ptr;
}

void *checked_calloc(size_t nmemb, size_t size) {
    void *ptr = calloc(nmemb, size);

    if (ptr == NULL) {
        fprintf(stderr, "Internal Error: Out of memory, allocation failure\n");
        exit(EXIT_FAILURE);
    }

    return ptr;
}
