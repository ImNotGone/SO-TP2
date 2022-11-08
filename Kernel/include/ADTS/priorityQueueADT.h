#ifndef P_QUEUEADT_H
#define P_QUEUEADT_H
#include <stdint.h>
#include <libs/memoryManager.h>
#include <lib.h>


typedef struct pQueueCDT * pQueueADT;
typedef int64_t (*compareFp)(void * e1, void * e2);

// Function to Create A New priorityQueue
pQueueADT newPQueue(compareFp comparePrio, compareFp compareElem, uint64_t elementSize, uint64_t prioritySize);

// Return the value at head
bool peekPq(pQueueADT pQueue, void* element);

// Removes the element with the
// highest priority from the list
bool popPq(pQueueADT pQueue, void* element);

// Function to push according to priority
bool pushPq(pQueueADT pQueue, void * elem, void * priority);

// Function to check is list is empty
int isEmptyPq(pQueueADT pQueue);

// Gets all the elements in the queue that are less than or equal to the given priority
// Returns an array of the elements found
// The size of the array is stored in the size parameter
void * getElementsLessThan(pQueueADT pQueue, void * priority, uint64_t * size);

// Remove an element from the queue
bool removePq(pQueueADT pQueue, void * elem);
#endif

