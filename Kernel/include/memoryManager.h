#ifndef __MEMORY_MANAGER_H
#define __MEMORY_MANAGER_H

#include <stdint.h>
#include <stddef.h>

void minit(void * start, size_t size);
void * malloc(size_t size);
void free(void *ptr);
void * calloc(size_t nmemb, size_t size);
void * realloc(void *ptr, size_t size);

#endif//__MEMORY_MANAGER_H
