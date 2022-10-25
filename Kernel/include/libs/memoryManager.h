#ifndef __MEMORY_MANAGER_H
#define __MEMORY_MANAGER_H

#include <stddef.h>
#include <stdint.h>
#include <types.h>
#define HEAP_SIZE 0x10000000 // 256 MiB

void minit(void *start);
void *malloc(uint64_t size);
void free(void *ptr);
void *calloc(uint64_t nmemb, uint64_t size);
void *realloc(void *ptr, uint64_t size);
void meminfo(TMemInfo *memInfo);

#endif //__MEMORY_MANAGER_H
