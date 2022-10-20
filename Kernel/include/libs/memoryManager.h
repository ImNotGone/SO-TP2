#ifndef __MEMORY_MANAGER_H
#define __MEMORY_MANAGER_H

#include <stdint.h>
#include <stddef.h>
#include <types.h>

void minit(void * start, uint64_t size);
void * malloc(uint64_t size);
void free(void *ptr);
void * calloc(uint64_t nmemb, uint64_t size);
void * realloc(void *ptr, uint64_t size);
void meminfo(TMemInfo* memInfo);

#endif//__MEMORY_MANAGER_H
