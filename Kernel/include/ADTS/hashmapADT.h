#ifndef HASHMAP_H
#define HASHMAP_H

#include <types.h>

typedef uint64_t (*hashFp)(void *);
typedef struct hashMapCDT * hashMapADT;

hashMapADT newHashMap(uint64_t keySize, uint64_t valueSize, hashFp prehash);

void putHm(hashMapADT hm, void * key, void * value);

bool removeHm(hashMapADT hm, void * key);

bool findHm(hashMapADT hm, void * key, void * value);

uint64_t sizeHm(hashMapADT hm);

void freeHashMap(hashMapADT hm);

void ** valuesHm(hashMapADT hm, uint64_t * size);

void ** keysHm(hashMapADT hm, uint64_t * size);

// ========== AUX ===========
uint64_t hashInt(void * key);

#endif
