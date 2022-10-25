// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifdef MM_BUDDY
/*
 * Buddy allocator
 *
 * Advantages:
 *  Easy to implement.
 *  Fast to allocate and de-allocate memory.
 *
 * Disadvantages:
 *  All alocation units are powers of 2.
 *  Internal fragmentation due to the first point.
 *  For example if the user requests 9 bytes,
 *  I have to give him a 16 byte block.
 *
 */

#include <libs/memoryManager.h>
#include <lib.h>
#include <types.h>

#define MULT_WILL_OF(a,b) (((a) > 0) && ((b) > (heapSize/(a))))

// Binary tree array implementation
//              A
//          B       C
//      D      E F      G
//
// = ['A', 'B', 'C', 'D', 'E', 'F', 'G']
//     0    1    2    3    4    5    6
// nodeQty = 2^3 - 1
// nodesInLastLevel = 2^2 (IMPORTANT)

typedef struct Node{
    struct Node * prev;
    struct Node * next;
} TNode;

typedef TNode * TList;

#define TWO_TO_THE(x) ((uint64_t)1 << (x))

#define MIN_ALLOC_LOG2 6    // 64B
#define MIN_ALLOC TWO_TO_THE(MIN_ALLOC_LOG2)

#define MAX_ALLOC_LOG2 28   // 256MB
#define MAX_ALLOC TWO_TO_THE(MAX_ALLOC_LOG2)

#define NODES (TWO_TO_THE(MAX_ALLOC_LOG2 - MIN_ALLOC_LOG2 + 1) - 1)

static TNode btree[NODES];   // Binary tree array
static uint8_t isSplitNode[TWO_TO_THE(MAX_ALLOC_LOG2 - MIN_ALLOC_LOG2)/8];

static void * heapStart  = NULL;
static uint64_t heapSize = 0;

static inline uint64_t rightChildIndex(uint64_t index);
static inline uint64_t leftChildIndex(uint64_t index);
static inline uint64_t parentIndex(uint64_t index);
static inline uint64_t siblingIndex(uint64_t index);
static uint8_t getNodeLevel(uint64_t request);

void minit(void * start, uint64_t size) {
    // TODO: ERROR
    if(size != HEAP_SIZE)
        return;

    // TODO: CALCULATE NODES NEEDED FOR GIVEN MEMORY
    // TODO: UPDATE GLOBAL VARIABLES
    heapStart = start;
    return;
}

void * malloc(uint64_t size) {
    void * out = NULL;
    // TODO: HANDLE ERRORS

    // TODO: FIND FREE BLOCK OF SIZE size
    getNodeLevel(size + sizeof(TNode));

    // TODO: UPDATE BLOCK FLAGS
    // TODO: RETRIEVE DIRECTION
    return out;
}

void free(void *ptr) {
    if(ptr == NULL) {
        return;
    }
    // TODO: FIND BLOCK WITH ptr ADDRESS
    // TODO: UPDATE BLOCK FLAGS
    // TODO: COMPRESS BUDDY IF POSSIBLE
    return;
}

void * calloc(uint64_t nmemb, uint64_t size) {
    void * out = NULL;
    if(!MULT_WILL_OF(nmemb, size)) {
        out = malloc(nmemb*size);
        if(out != NULL) {
            memset(out, 0, nmemb*size);
        }
    }
    return out;
}

void * realloc(void *ptr, uint64_t size) {
    if(ptr == NULL) {
        return malloc(size);
    }
    void * out = malloc(size);
    if(out != NULL) {
        memcpy(out, ptr, size);
        free(ptr);
    }
    return out;
}

// ========== BUDDY ==========
static inline uint64_t rightChildIndex(uint64_t index) {
    return (index << 1) + 1;
}
static inline uint64_t leftChildIndex(uint64_t index) {
    return (index << 1) + 2;
}
static inline uint64_t parentIndex(uint64_t index) {
    return ((index - 1) >> 1);
}
static inline uint64_t siblingIndex(uint64_t index) {
    return ((index - 1) ^ 1) + 1;
}

static uint8_t getNodeLevel(uint64_t request) {
    uint8_t level = MAX_ALLOC_LOG2 - MIN_ALLOC_LOG2;
    uint64_t size = MIN_ALLOC;

    while(size < request) {
        level--;
        size <<= 1;
    }

    return level;
}

#endif//USE_BUDDY
