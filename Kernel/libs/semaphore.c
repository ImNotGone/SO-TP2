// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libs/semaphore.h>


// ====================== Definitions =================================

// Semaphore data
static semCollectionADT semCollection = NULL;

// Auxiliary functions
static void acquire(lock_t *lock);
static void release(lock_t *lock);

// ==================== Public Functions ====================
// Creates semaphore manager
void initSemaphores() { semCollection = newSemCollection(); }

// Free semaphore manager
void freeSemaphores() { freeSemCollection(semCollection); }

// Creates a new semaphore with the given name and initial value
// If the semaphore already exists, it returns the existing one
sem_t sem_open(const char *name, uint32_t value) {
    return getSem(semCollection, name, value);
}

// Closes the semaphore
int64_t sem_close(sem_t sem) {
    // Check if the semaphore exists
    if (!semExists(semCollection, sem)) {
        return -1;
    }

    return closeSem(semCollection, sem);
}

// Unlinks the semaphore, if there are no processes attached to it removes it
int64_t sem_unlink(const char *name) {
    return unlinkSem(semCollection, name);
}

// Waits for the semaphore to be available
// If the semaphore is available, it decrements it and returns 0
// If the semaphore is not available, it blocks the process
int64_t sem_wait(sem_t sem) {

    // Check if the semaphore exists
    if (!semExists(semCollection, sem)) {
        return -1;
    }

    // Get the semaphore data
    lock_t semLock = getSemLock(semCollection, sem);
    uint64_t semValue = getSemValue(semCollection, sem);

    // Lock the semaphore
    acquire(&semLock);

    // Check if the semaphore is available
    if (semValue > 0) {

        // Semaphore available
        decSemValue(semCollection, sem);

        // Unlock the semaphore
        release(&semLock);

        return 0;
    }

    // Semaphore not available
    // Add the process to the waiting queue
    addWaitingProcess(semCollection, sem, getActivePid());

    // Unlock the semaphore
    release(&semLock);

    // Block the process
    blockProcess(getActivePid());

    return 0;
}

// Increments the semaphore value
// If there are processes waiting for the semaphore, it unblocks one of them
// If there are no processes waiting for the semaphore, it increments the value
int64_t sem_post(sem_t sem) {

    // Check if the semaphore exists
    if (!semExists(semCollection, sem)) {
        return -1;
    }

    // Get the semaphore data
    lock_t semLock = getSemLock(semCollection, sem);

    // Lock the semaphore
    acquire(&semLock);

    // Check if there are processes waiting for the semaphore
    if (getWaitingQueueSize(semCollection, sem) > 0) {

        // Unblock a process
        pid_t pid = getNextWaitingProcess(semCollection, sem);
        unblockProcess(pid);

        // Unlock the semaphore
        release(&semLock);

        return 0;
    }

    // No processes waiting for the semaphore
    // Increment the value
    incSemValue(semCollection, sem);

    // Unlock the semaphore
    release(&semLock);

    return 0;
}

// Gets data from the semaphore collection
TSemInfo *sem_info(uint64_t *size) {
    return semCollectionInfo(semCollection, size);
}

// --------------------- Unnamed Semaphores ---------------------
// Creates a new unnamed semaphore with the given initial value
// Returns the semaphore id
sem_t sem_init(uint64_t value) {
    return initUnnamedSem(semCollection, value);
}

// Destroys the semaphore
int64_t sem_destroy(sem_t sem) {

    // Check if the semaphore exists
    if (!semExists(semCollection, sem)) {
        return -1;
    }

    return destroyUnnamedSem(semCollection, sem);
}

// ==================== Auxiliary Functions ====================
// Acquire the lock
static void acquire(lock_t *lock) {
    while (_xchg(lock, 1) != 0);
}

// Release the lock
static void release(lock_t *lock) { _xchg(lock, 0); }

