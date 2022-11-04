#include <ADTS/hashmapADT.h>
#include <libs/memoryManager.h>
#include <lib.h>

typedef struct entry{
    void * key;
    void * value;
    enum {FREE = 0, USED, BRIDGE} status;
} entry;

typedef struct hashMapCDT {
    uint64_t totalSize;
    uint64_t usedSize;
    double threshold;
    hashFp prehash;
    entry * lookup;
    uint64_t lookupSize;
    uint64_t keySize;
    uint64_t valueSize;
} hashMapCDT;

#define INITIAL_SIZE 20

hashMapADT newHashMap(uint64_t keySize, uint64_t valueSize, hashFp prehash) {
    hashMapADT new = malloc(sizeof(hashMapCDT));
    if(new == NULL || (new->lookup = calloc(INITIAL_SIZE, sizeof(entry))) == NULL) {
        free(new);
        return NULL;
    }
    new->totalSize = 0;
    new->usedSize = 0;
    new->threshold = 0.75;
    new->prehash = prehash;
    new->lookupSize = INITIAL_SIZE;
    new->keySize = keySize;
    new->valueSize = valueSize;
    return new;
}

static uint64_t hash(hashMapADT hm, void * key) {
    return hm->prehash(key) % hm->lookupSize;
}

static void resize(hashMapADT hm) {
    entry * oldTable = hm->lookup;
    hm->lookup = calloc(hm->lookupSize*2, sizeof(entry));
    if(hm->lookup == NULL){
        hm->lookup = oldTable;
        return;
    }
    uint64_t oldSize = hm->lookupSize;
    hm->lookupSize *= 2;
    hm->totalSize = 0;
    hm->usedSize = 0;
    for(uint64_t i = 0; i < oldSize; i++) {
        if(oldTable[i].status == USED) {
            putHm(hm, oldTable[i].key, oldTable[i].value);
            free(oldTable[i].key);
            free(oldTable[i].value);
        }
    }
    free(oldTable);
}

void putHm(hashMapADT hm, void * key, void * value) {
    if(hm == NULL || key == NULL || value == NULL) {
        return;
    }

    bool updated = removeHm(hm, key);

    // ===== CREATE NEW ENTRY =====
    entry newEntry;

    newEntry.key = malloc(hm->keySize);
    memcpy(newEntry.key, key, hm->keySize);

    newEntry.value = malloc(hm->valueSize);
    memcpy(newEntry.value, value, hm->valueSize);

    newEntry.status = USED;
    // =====

    uint64_t pos = hash(hm, key);
    bool found = false;
    if(hm->lookup[pos].status == USED) {
        for(uint64_t i = 1; i < hm->lookupSize && !found; i++) {
            uint64_t index = (pos+i) % hm->lookupSize;
            if(hm->lookup[index].status != USED) {
                pos = index;
                found = true;
            }
        }
    }
    hm->usedSize++;
    if(!updated) {
        hm->totalSize++;
    }
    hm->lookup[pos] = newEntry;
    bool mustResize = ((double)hm->usedSize/hm->lookupSize) > hm->threshold;
    if(mustResize) {
        resize(hm);
    }
}

static bool keyEquals(hashMapADT hm, void * key1, void * key2) {
    return memcmp(key1, key2, hm->keySize) == 0;
}

bool removeHm(hashMapADT hm, void * key) {
    if(key == NULL) return false;

    uint64_t pos = hash(hm, key);
    for(uint64_t i = 0; i < hm->lookupSize; i++) {
        uint64_t index = (pos + i) % hm->lookupSize;
        entry * aux = &hm->lookup[index];
        if(aux->status == FREE) {
            return false;
        }
        if(aux->status == USED && keyEquals(hm, aux->key, key)) {
            if(hm->lookup[(index+1) % hm->lookupSize].status == FREE) {
                aux->status = FREE;
                hm->totalSize--;
            } else {
                aux->status = BRIDGE;
            }
            free(aux->key);
            free(aux->value);
            hm->usedSize--;
            return true;
        }
    }
    return false;
}


bool findHm(hashMapADT hm, void * key, void * value) {
    if(key == NULL || value == NULL) return false;

    uint64_t pos = hash(hm, key);
    for(uint64_t i = 0; i < hm->lookupSize; i++) {
        uint64_t index = (pos + i) % hm->lookupSize;
        entry * aux = &hm->lookup[index];
        if(aux->status == FREE) return false;
        if(aux->status == USED && keyEquals(hm, aux->key, key)) {
            memcpy(value, aux->value, hm->valueSize);
            return true;
        }
    }
    return false;
}

uint64_t sizeHm(hashMapADT hm) {
    return hm->usedSize;
}

void freeHashMap(hashMapADT hm) {
    for(uint64_t i = 0; i < hm->lookupSize; i++) {
        entry * aux = &hm->lookup[i];
        if(aux->status == USED) {
            free(aux->key);
            free(aux->value);
        }
    }
    free(hm->lookup);
    free(hm);
}

// ================== AUX ======================

uint64_t hashInt(void * key) {
    uint64_t aux = *(uint64_t *) key;
    /* Robert Jenkins' 32 bit Mix Function */
	aux += (aux << 12);
	aux ^= (aux >> 22);
	aux += (aux << 4);
	aux ^= (aux >> 9);
	aux += (aux << 10);
	aux ^= (aux >> 2);
	aux += (aux << 7);
	aux ^= (aux >> 12);

	/* Knuth's Multiplicative Method */
	aux = (aux >> 3) * 2654435761;
    return aux;
}

