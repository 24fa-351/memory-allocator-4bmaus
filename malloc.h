#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>

// Public functions
void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_realloc(void *ptr, size_t size);

// Debugging function
void print_heap_status(void);

#endif // MALLOC_H
