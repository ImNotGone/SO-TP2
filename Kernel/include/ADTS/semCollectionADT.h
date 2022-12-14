#ifndef SEMCOLLECTIONADT_H
#define SEMCOLLECTIONADT_H

#include <types.h>
#include <ADTS/queueADT.h>
#include <libs/string.h>

#define MAX_SEM 64

typedef struct semCollectionCDT *semCollectionADT;


// Create a new semaphore collection
semCollectionADT newSemCollection();

// Check if the semaphore exists
int semExists(semCollectionADT semCollection, sem_t sem);

// Get a semaphore from the collection, if it exists or create a new one
sem_t getSem(semCollectionADT semCollection, const char *name, uint64_t value);

// Get unnamed semaphore
sem_t initUnnamedSem(semCollectionADT semCollection, uint64_t value);

// Get the semaphore value
uint64_t getSemValue(semCollectionADT semCollection, sem_t sem);

// Gets the amount of processes waiting for the semaphore
int getWaitingQueueSize(semCollectionADT semCollection, sem_t sem);

// Increment the semaphore value
int incSemValue(semCollectionADT semCollection, sem_t sem);

// Decrement the semaphore value
int decSemValue(semCollectionADT semCollection, sem_t sem);

// Get semaphore lock
lock_t getSemLock(semCollectionADT semCollection, sem_t sem);

// Close the semaphore
int64_t closeSem(semCollectionADT semCollection, sem_t sem);

// Unlink a semaphore
int64_t unlinkSem(semCollectionADT semCollection, const char *name);

// Destroy an unnamed semaphore
int64_t destroyUnnamedSem(semCollectionADT semCollection, sem_t sem);

// Add a process to the waiting queue
int addWaitingProcess(semCollectionADT semCollection, sem_t sem, pid_t pid);

// Get the next process in the waiting queue
pid_t getNextWaitingProcess(semCollectionADT semCollection, sem_t sem);

// Free the semaphore collection
void freeSemCollection(semCollectionADT semCollection);

// Dump the semaphore collection
TSemInfo *semCollectionInfo(semCollectionADT semCollection, uint64_t *size);

// Returns a copy of the waiting queue
pid_t * getWaitingQueue(semCollectionADT semCollection, sem_t sem, uint64_t *size);

#endif //SEMCOLLECTIONADT_H
