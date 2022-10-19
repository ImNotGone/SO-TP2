#ifndef __MEMORY_MANAGER_H
#define __MEMORY_MANAGER_H

#include <stdint.h>
#include <stddef.h>

void minit(void * start, uint64_t size);
void * malloc(uint64_t size);
void free(void *ptr);
void * calloc(uint64_t nmemb, uint64_t size);
void * realloc(void *ptr, uint64_t size);

#endif//__MEMORY_MANAGER_H
