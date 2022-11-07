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

#define SPLIT   0x1
#define USED    0x2

typedef struct Node{
    uint8_t flag;
} TNode;

#define TWO_TO_THE(x) ((uint64_t)1 << (x))

#define MIN_ALLOC_LOG2 6    // 64B
#define MIN_ALLOC TWO_TO_THE(MIN_ALLOC_LOG2)

#define MAX_ALLOC_LOG2 28   // 256MB
#define MAX_ALLOC TWO_TO_THE(MAX_ALLOC_LOG2)

#define NODES (TWO_TO_THE(MAX_ALLOC_LOG2 - MIN_ALLOC_LOG2 + 1) - 1)

static TNode * btree;   // Binary tree array

static uint8_t * heapStart  = NULL;
static uint64_t heapSize = 0;
static uint64_t usedMemory = 0;

static inline uint64_t rightChildIndex(uint64_t index);
static inline uint64_t leftChildIndex(uint64_t index);
static inline uint64_t parentIndex(uint64_t index);
static inline uint64_t siblingIndex(uint64_t index);
static inline uint64_t getFirstNodeIndex(uint8_t level);
static uint8_t levelForRequest(uint64_t request);
static int64_t getFreeNodeIndex(uint8_t level);
static uint8_t * getAddressAtIndex(uint64_t index, uint8_t level);
static int64_t getIndexAtAdress(uint8_t * ptr, uint8_t * level);
static uint8_t levelForAdress(void * ptr);

static inline bool isFlag(uint64_t index, uint8_t flag);
static inline void flipFlag(uint64_t index, uint8_t flag);
static inline void setFlag(uint64_t index, uint8_t flag);
static inline void unsetFlag(uint64_t index, uint8_t flag);

static void setSplitParent(uint64_t index);
static void setUsedChildren(uint64_t index);


void minit(void * start, uint64_t size) {
    if(size < HEAP_SIZE + HEAP_STRUCTURE_SIZE) {
        return;
    }

    heapStart = start;
    heapSize = HEAP_SIZE;
    btree = (TNode *) heapStart + HEAP_SIZE;
    return;
}

void * malloc(uint64_t size) {
    if(size > heapSize || size == 0) {
        return NULL;
    }

    uint8_t level = levelForRequest(size);

    // should not happen, since i checked the size before
    if(level > MAX_ALLOC_LOG2) {
        return NULL;
    }

    int64_t index = getFreeNodeIndex(level);

    if(index == -1) {
        return NULL;
    }

    setSplitParent(index);
    setUsedChildren(index);
    setFlag(index, USED);
    usedMemory += TWO_TO_THE(level);
    return getAddressAtIndex(index, level);
}

void free(void *ptr) {
    if(ptr == NULL) {
        return;
    }
    uint8_t level = levelForAdress(ptr);
    uint64_t index = getIndexAtAdress(ptr, &level);
    // FLIP USED FLAG
    flipFlag(index, USED);
    bool indexChanged = true;
    while (index != 0 && indexChanged) {
        uint64_t sibling = siblingIndex(index);
        indexChanged = false;
        if(!(isFlag(sibling, USED) || isFlag(sibling, SPLIT))) {
            index = parentIndex(index);
            unsetFlag(index, SPLIT);
            indexChanged = true;
        }
    }
    usedMemory -= TWO_TO_THE(level);
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

void meminfo(TMemInfo* memInfo) {
    memInfo->total = heapSize;
    memInfo->used = usedMemory;
    memInfo->free = heapSize - usedMemory;
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

// level:28 -> index:0
// level:27 -> index:1
// level:26 -> index:3
// level:25 -> index:7
// 2^(MAX_ALLOC_LOG2 - level) - 1
static inline uint64_t getFirstNodeIndex(uint8_t level) {
    return TWO_TO_THE(MAX_ALLOC_LOG2 - level) - 1;
}

static uint8_t * getAddressAtIndex(uint64_t index, uint8_t level) {
    return heapStart + ((index - (TWO_TO_THE(MAX_ALLOC_LOG2 - level) -1) ) << level);
}

static int64_t getIndexAtAdress(uint8_t * ptr, uint8_t * level) {
    uint64_t index = 0;
    uint8_t aux = *level + 1;
    do {
        aux--;
        index = ((ptr - heapStart) >> (aux)) + TWO_TO_THE(MAX_ALLOC_LOG2 - aux) - 1;
    } while(!isFlag(index, USED));

    *level = aux;

    return index;
}


static uint8_t levelForAdress(void * ptr) {
    uint64_t addr = (uint8_t *)ptr - heapStart;
    uint8_t level = MAX_ALLOC_LOG2;
    uint64_t size = MAX_ALLOC;
    while(addr % size != 0) {
        level--;
        size >>= 1;
    }
    return level;
}

static uint8_t levelForRequest(uint64_t request) {
    uint8_t level = MIN_ALLOC_LOG2;
    uint64_t size = MIN_ALLOC;

    while(size < request) {
        level++;
        size <<= 1;
    }

    return level;
}

static int64_t getFreeNodeIndex(uint8_t level) {
    uint64_t index = getFirstNodeIndex(level);
    uint64_t lastIndex = getFirstNodeIndex(level - 1);

    while( index < lastIndex && (isFlag(index, USED) || isFlag(index, SPLIT))) {
        index++;
    }

    if(index == lastIndex) {
        return -1;
    }

    return index;
}

static inline bool isFlag(uint64_t index, uint8_t flag) {
    return btree[index].flag & flag;
}

static inline void flipFlag(uint64_t index, uint8_t flag) {
    btree[index].flag ^= flag;
}

static inline void setFlag(uint64_t index, uint8_t flag) {
    btree[index].flag |= flag;
}

static inline void unsetFlag(uint64_t index, uint8_t flag) {
    btree[index].flag &= ~flag;
}

static void setSplitParent(uint64_t index) {
    while(index != 0) {
        index = parentIndex(index);
        setFlag(index, SPLIT);
    }
}
static void setUsedChildren(uint64_t index) {
    if(index > NODES) return;
    setFlag(index, USED);
    setUsedChildren(leftChildIndex(index));
    setUsedChildren(rightChildIndex(index));
}
#endif//USE_BUDDY
