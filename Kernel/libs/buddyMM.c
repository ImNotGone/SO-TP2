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

#include <memoryManager.h>
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
    bool used;
} TNode;

static TNode * btree;   // Binary tree array
static inline uint64_t getRightChild(uint64_t index) {
    return (index<<1)+1;
}
static inline uint64_t getLeftChild(uint64_t index) {
    return (index<<1)+2;
}
// 2^x = 1<<x;

static void * heapStart;
static uint64_t heapSize;

void minit(void * start, uint64_t size) {
    // TODO: ALIGN memory
    // TODO: CHECK MEMORY SIZE
    // TODO: CALCULATE NODES NEEDED FOR GIVEN MEMORY
    // TODO: UPDATE GLOBAL VARIABLES

    return;
}

void * malloc(uint64_t size) {
    void * out = NULL;
    // TODO: HANDLE ERRORS

    // TODO: FIND FREE BLOCK OF SIZE size
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
    void * out = malloc(size);
    if(out != NULL) {
        memcpy(out, ptr, size);
    }
    free(ptr);
    return out;
}

#endif//USE_BUDDY
