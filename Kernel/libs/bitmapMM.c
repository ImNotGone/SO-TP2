// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifdef MM_BITMAP
#include <libs/memoryManager.h>
#include <string.h>


#define MULT_WILL_OF(a, b) (((a) > 0) && ((b) > (blocks * FACTOR / (a))))

// Bitmap states
enum {
    FREE = 0,
    USED = 1,
    BOUNDARY = 2
};


// Arena of memory, it has the memory blocks to be returned
static void *arena;

// Bitmap of arena, it contains the state of each block
static uint8_t *bitmap;

// Blocks of memory
static uint64_t blocks;

// Info of memory
static uint64_t usedBlocks;

// Initialize memory manager
void minit(void *start, uint64_t size) {

    if(size < HEAP_SIZE + HEAP_STRUCTURE_SIZE) {
        return;
    }

    // Calculate the number of blocks needed
    blocks = (size - HEAP_STRUCTURE_SIZE) / FACTOR;

    // Assign bitmap and Arena
    bitmap = (uint8_t*) start;
    arena = bitmap + blocks;

    // Set all blocks to free
    for (uint64_t i = 0; i < blocks; i++) {
        bitmap[i] = FREE;
    }
}

// Allocate memory
void *malloc(uint64_t size) {

    if (size == 0) {
        return NULL;
    }

    // Calculate the number of blocks needed for the allocation rounded up
    uint64_t blocksNeeded = size / FACTOR + ((size % FACTOR) ? 1 : 0);

    // Find the free blocks needed
    uint64_t currentBlock = 0;
    while(currentBlock <= blocks - blocksNeeded) {

        // Count the number of free blocks from currentBlock
        uint64_t freeBlocks = 0;
        bool found = false;
        for (uint64_t i = currentBlock; i < blocks; i++, freeBlocks++) {

            // Found the blocks needed
            if (freeBlocks == blocksNeeded) {
                found = true;
                break;
            }

            // The block is being used
            if (bitmap[i] != FREE) {
                break;
            }
        }

        // We found the blocks blocks needed
        if (found) {

            // Mark the first block as a boundary
            bitmap[currentBlock] = BOUNDARY;

            // Mark the blocks as used
            for (uint64_t i = currentBlock + 1; i < currentBlock + blocksNeeded; i++) {
                bitmap[i] = USED;
            }

            // Update the number of used blocks
            usedBlocks += blocksNeeded;

            // Return the address of the first block
            return (uint8_t*)arena + currentBlock * FACTOR;
        }

        // Continue searching
        currentBlock += freeBlocks + 1;
    }

    // No blocks found
    return NULL;
}

// Calloc
void *calloc(uint64_t nmemb, uint64_t size) {

    void *out = NULL;

    if(!MULT_WILL_OF(nmemb, size)) {
        out = malloc(nmemb * size);

        if (out != NULL) {
            memset(out, 0, nmemb * size);
        }
    }

    return out;
}

// Free memory
void free(void *ptr) {

    if (ptr == NULL) {
        return;
    }

    // Calculate the pointer offset in the arena
    uint64_t arenaOffset = (uint64_t) ((uint8_t*)ptr - (uint8_t*)arena);

    // Calculate the block index
    uint64_t blockIndex = arenaOffset / FACTOR;

    if (blockIndex >= blocks) {
        return;
    }

    if (arenaOffset % FACTOR != 0) {
        return;
    }

    if (bitmap[blockIndex] != BOUNDARY) {
        return;
    }


    // Mark the boundary as free
    bitmap[blockIndex] = FREE;

    // Mark the blocks as free until we reach a free block or a boundary
    uint64_t i;
    for (i = blockIndex + 1; i < blocks && bitmap[i] == USED; i++) {
        bitmap[i] = FREE;
    }

    usedBlocks -= i - blockIndex;
}

// Reallocate memory
void *realloc(void *ptr, uint64_t size) {


    if (ptr == NULL) {
        return malloc(size);
    }

    if (size == 0) {
        return NULL;
    }

    // Allocate new memory
    void *newPtr = malloc(size);

    if (newPtr == NULL) {
        return NULL;
    }


    // Copy the old data to the new memory
    memcpy(newPtr, ptr, size);

    // Free the old memory
    free(ptr);

    return newPtr;
}

// Get memory info
void meminfo(TMemInfo* memInfo) {
    memInfo->total = blocks * FACTOR;
    memInfo->used = usedBlocks * FACTOR;
    memInfo->free = (blocks - usedBlocks) * FACTOR;
}
#endif
