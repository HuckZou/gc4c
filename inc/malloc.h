/*
 * malloc.h
 *
 * Header file for malloc functions
 *
 */

#define malloc(size) gc_malloc(size)
#define realloc(ptr, size) gc_malloc(ptr, size)
#define calloc(nmemb, size) gc_calloc(nmemb, size)
#define free(ptr) gc_free(ptr)

#include <stddef.h>

void* gc_malloc(size_t size);
void* gc_realloc(void* ptr, size_t size);
void* gc_free(void* ptr);
void* gc_calloc(size_t nmemb, size_t size);
