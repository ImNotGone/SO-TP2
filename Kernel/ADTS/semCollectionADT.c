#include <ADTS/semCollectionADT.h>


// ====================== Definitions =================================

typedef struct semData {
    // Semaphore
    sem_t sem;

    // Semaphore name
    const char *name;
    bool isLinked;

    // Semaphore value
    uint64_t value;

    // Semaphore lock
    lock_t lock;

    // Attached processes
    uint64_t attachedProcesses;


    // Waiting queue
    queueADT waitingQueue;
    uint64_t waitingQueueSize;

} semdata_t;


typedef struct semCollectionCDT {
    // Semaphore collection
    // TODO: maybe use a hash map?
    semdata_t *semaphores[MAX_SEM];
    uint64_t semaphoresSize;
} semCollectionCDT;


// ==================== Functions ====================

// ------------------- New -------------------

// Create a new semaphore collection
semCollectionADT newSemCollection() {

    semCollectionADT semCollection = calloc(1, sizeof(semCollectionCDT));

    return semCollection;
};

// ------------------- Getters -------------------

// Check if the semaphore exists
int semExists(semCollectionADT semCollection, sem_t sem) {

    // Check if the semaphore exists
    if (sem < 0 || sem >= MAX_SEM || semCollection->semaphores[sem] == NULL) {
        return false;
    }

    return true;
}

// Get a semaphore from the collection, if it exists or create a new one
sem_t getSem(semCollectionADT semCollection, const char *name, uint64_t initialValue) {

    // Search for the semaphore
    int firstFree = -1;
    for (int i = 0; i < MAX_SEM; i++) {
    
        // Semaphore found
        if (semCollection->semaphores[i] != NULL && semCollection->semaphores[i]->isLinked && strcmp(semCollection->semaphores[i]->name, name) == 0) {

            semCollection->semaphores[i]->attachedProcesses++;

            return semCollection->semaphores[i]->sem;

        } else if (firstFree == -1 && semCollection->semaphores[i] == NULL) {
            
            // First free position
            firstFree = i;
        }
    }

    // If the semaphore doesn't exist, create a new one

    // No free position
    if (firstFree == -1) {
        return -1;
    }

    // Create the semaphore
    semdata_t *semData = calloc(1, sizeof(semdata_t));

    semData->name = name;
    semData->isLinked = true;

    semData->value = initialValue;

    semData->attachedProcesses = 1;

    semData->waitingQueue = newQueue(sizeof(int));
    semData->waitingQueueSize = 0;
    
    // SemId
    semData->sem = firstFree;

    // Add the semaphore to the collection
    semCollection->semaphores[firstFree] = semData;
    semCollection->semaphoresSize++;

    return semData->sem;
};


// Get the semaphore value
uint64_t getSemValue(semCollectionADT semCollection, sem_t sem) {

    // Check if the semaphore exists
    if (!semExists(semCollection, sem)) {
        return -1;
    }

    return semCollection->semaphores[sem]->value;
};

// Get semaphore lock
lock_t getSemLock(semCollectionADT semCollection, sem_t sem) {

    // Check if the semaphore exists
    if (!semExists(semCollection, sem)) {
        return -1;
    }

    return semCollection->semaphores[sem]->lock;
};

// Gets the amount of processes waiting for the semaphore
int getWaitingQueueSize(semCollectionADT semCollection, sem_t sem) {

    // Check if the semaphore exists
    if (!semExists(semCollection, sem)) {
        return -1;
    }

    return semCollection->semaphores[sem]->waitingQueueSize;
};

// ------------------- Setters -------------------

// Increment the semaphore value
int incSemValue(semCollectionADT semCollection, sem_t sem) {

    // Check if the semaphore exists
    if (!semExists(semCollection, sem)) {
        return -1;
    }

    semCollection->semaphores[sem]->value++;

    return 0;
};

// Decrement the semaphore value
int decSemValue(semCollectionADT semCollection, sem_t sem) {

    // Check if the semaphore exists
    if (!semExists(semCollection, sem)) {
        return -1;
    }

    semCollection->semaphores[sem]->value--;

    return 0;
};

// Add a process to the waiting queue
int addWaitingProcess(semCollectionADT semCollection, sem_t sem, int pid) {

    // Check if the semaphore exists
    if (!semExists(semCollection, sem)) {
        return -1;
    }

    queue(semCollection->semaphores[sem]->waitingQueue, &pid);
    semCollection->semaphores[sem]->waitingQueueSize++;

    return 0;
};

// Get the next process in the waiting queue
int getNextWaitingProcess(semCollectionADT semCollection, sem_t sem) {

    // Check if the semaphore exists
    if (!semExists(semCollection, sem)) {
        return -1;
    }

    int pid;
    dequeue(semCollection->semaphores[sem]->waitingQueue, &pid);
    semCollection->semaphores[sem]->waitingQueueSize--;
    return pid;
};


// Close a semaphore
int64_t closeSem(semCollectionADT semCollection, sem_t sem) {

    // If sem is not valid return -1
    if (!semExists(semCollection, sem)) {
        return -1;
    }

    // Decrement the attached processes
    semCollection->semaphores[sem]->attachedProcesses--;

    return 0;
};

// Unlink a semaphore
int64_t unlinkSem(semCollectionADT semCollection, const char *name) {

    // Search for the semaphore
    int sem = -1;
    for (int i = 0; i < MAX_SEM; i++) {
        // Semaphore found
        if (semCollection->semaphores[i] != NULL && semCollection->semaphores[i]->isLinked && strcmp(semCollection->semaphores[i]->name, name) == 0) {
            sem = i;
            break;
        }
    }

    // No semaphore found
    if (sem == -1) {
        return -1;
    }

    // Set the semaphore as unlinked
    semCollection->semaphores[sem]->isLinked = false;

    // If there are no more attached processes free the semaphore
    if (semCollection->semaphores[sem]->attachedProcesses == 0) {

        // Free the semaphore
        free(semCollection->semaphores[sem]->waitingQueue);
        free(semCollection->semaphores[sem]);

        // Remove the semaphore from the collection
        semCollection->semaphores[sem] = NULL;
        semCollection->semaphoresSize--;
    }

    return 0;
};

// ------------------- Free -------------------
// Free the semaphore collection
void freeSemCollection(semCollectionADT semCollection) {
    
    // Free the semaphores
    for (int i = 0; i < MAX_SEM; i++) {
        if (semCollection->semaphores[i] != NULL) {

            // Free the waiting queue
            freeQueue(semCollection->semaphores[i]->waitingQueue);

            free(semCollection->semaphores[i]);
        }
    }

    free(semCollection);
};

// ------------------- Dump -------------------
// Dump the semaphore collection
TSemInfo *semCollectionInfo(semCollectionADT semCollection, uint64_t *size) {

    // Create the array
    TSemInfo *res = calloc(1, sizeof(TSemInfo) * semCollection->semaphoresSize);

    // Fill the array
    int i = 0;
    for (int j = 0; j < MAX_SEM; j++) {
        if (semCollection->semaphores[j] != NULL) {

            // Get semaphore data
            res[i].value = semCollection->semaphores[j]->value;
            res[i].name = semCollection->semaphores[j]->name;
            res[i].waitingQueueSize = semCollection->semaphores[j]->waitingQueueSize;

            // Get Blocked processes
            int *blockedProcesses = calloc(1, sizeof(int) * res[i].waitingQueueSize);

            // Iterate the queue
            toBegin(semCollection->semaphores[j]->waitingQueue);
            int k = 0;
            while (hasNext(semCollection->semaphores[j]->waitingQueue)) {
                next(semCollection->semaphores[j]->waitingQueue, &blockedProcesses[k]);
                k++;
            }

            res[i].waitingQueue = blockedProcesses;

            i++;
        }
    }

    *size = i;
    return res;
};
