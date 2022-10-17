#ifdef MM_BITMAP
#include <memoryManager.h>
#include <string.h>
#include <types.h>

// Factor of size of bitmap to size of memory
#define FACTOR 64

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
static unsigned char *bitmap;

// Blocks of memory
static unsigned int blocks;




// Initialize memory manager
void minit(void *start, uint64_t size) {

    // Calculate the number of blocks needed
    uint64_t blocks = size / FACTOR;

    if (blocks <= 0) {
        // TODO: Error
        return;
    }

    // Assign bitmap and Arena
    bitmap = (unsigned char*) start;
    arena = bitmap + blocks;

    // Set all blocks to free
    for (int i = 0; i < blocks; i++) {
        bitmap[i] = FREE;
    }
}

// Allocate memory
void *malloc(uint64_t size) {

    if (size <= 0) {
        // TODO: Error
        return NULL;
    }

    // Calculate the number of blocks needed for the allocation rounded up
    unsigned int blocksNeeded = size / FACTOR + ((size % FACTOR) ? 1 : 0);

    // Find the free blocks needed
    unsigned int currentBlock = 0;

    while(currentBlock <= blocks - blocksNeeded) {

        // Count the number of free blocks from currentBlock
        unsigned int freeBlocks = 0;
        for (int i = currentBlock; i < blocks; i++) {

            // The block is being used
            if (bitmap[i] != FREE) {
                break;
            }

            freeBlocks++;
        }

        // We found the blocks blocks needed
        if (freeBlocks == blocksNeeded) {

            // Mark the first block as a boundary
            bitmap[currentBlock] = BOUNDARY;

            // Mark the blocks as used
            for (int i = currentBlock + 1; i < currentBlock + blocksNeeded; i++) {
                bitmap[i] = USED;
            }

            // Return the address of the first block
            return (unsigned char*)arena + currentBlock * FACTOR;
        }

        // Continue searching
        currentBlock += freeBlocks + 1;
    }

    // No blocks found
    // TODO: Error
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
        // TODO: Error
        return;
    }

    // Calculate the pointer offset in the arena
    unsigned int arenaOffset = (unsigned int) ((unsigned char*)ptr - (unsigned char*)arena);

    // Calculate the block index
    unsigned int blockIndex = arenaOffset / FACTOR;

    if (blockIndex >= blocks) {
        // TODO: Error
        return;
    }

    if (arenaOffset % FACTOR != 0) {
        // TODO: Error
        return;
    }

    if (bitmap[blockIndex] != BOUNDARY) {
        // TODO: Error
        return;
    }


    // Mark the blocks as free until we reach a free block or a boundary
    for (int i = blockIndex; i < blocks && bitmap[i] == USED; i++) {
        bitmap[i] = FREE;
    }
}

// Reallocate memory
void *realloc(void *ptr, uint64_t size) {


    if (ptr == NULL) {
        // TODO: Error
        return NULL;
    }

    if (size <= 0) {
        // TODO: Error
        return NULL;
    }

    // Allocate new memory
    void *newPtr = malloc(size);

    if (newPtr == NULL) {
        // TODO: Error
        return NULL;
    }


    // Copy the old data to the new memory
    memcpy(newPtr, ptr, size);

    // Free the old memory
    free(ptr);

    return newPtr;
}

#endif
