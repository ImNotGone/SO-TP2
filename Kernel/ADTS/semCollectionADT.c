// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ADTS/semCollectionADT.h>

// ====================== Definitions =================================

#define BLOCK_SIZE 10

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
    semdata_t **semaphores;

    uint64_t semaphoresCapacity;
} semCollectionCDT;

// ==================== Functions ====================

// ------------------- CMP -------------------
static int64_t comparePid(void *pid1, void *pid2) {
    if (pid1 == NULL || pid2 == NULL) {
        return pid1 != NULL || pid2 != NULL;
    }
    pid_t a = *(pid_t *)pid1;
    pid_t b = *(pid_t *)pid2;
    return (a > b) - (a < b);
}

// ------------------- New -------------------

// Create a new semaphore collection
semCollectionADT newSemCollection() {

    semCollectionADT semCollection = calloc(1, sizeof(semCollectionCDT));

    if (semCollection == NULL) {
        return NULL;
    }

    semCollection->semaphores = calloc(1, sizeof(semdata_t *) * BLOCK_SIZE);
    semCollection->semaphoresCapacity = BLOCK_SIZE;

    return semCollection;
};

// ------------------- Getters -------------------

// Check if the semaphore exists
int semExists(semCollectionADT semCollection, sem_t sem) {

    if (semCollection == NULL) {
        return false;
    }

    // Check if the semaphore exists
    if (sem < 0 || sem >= semCollection->semaphoresCapacity ||
        semCollection->semaphores[sem] == NULL) {
        return false;
    }

    return true;
}

// Get a semaphore from the collection, if it exists or create a new one
sem_t getSem(semCollectionADT semCollection, const char *name,
             uint64_t initialValue) {

    if (semCollection == NULL || name == NULL) {
        return -1;
    }

    // Search for the semaphore
    int firstFree = -1;
    for (int i = 0; i < semCollection->semaphoresCapacity; i++) {

        // Semaphore found
        if (semCollection->semaphores[i] != NULL &&
            semCollection->semaphores[i]->isLinked &&
            strcmp(semCollection->semaphores[i]->name, name) == 0) {

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

        semdata_t **new = realloc(
            semCollection->semaphores,
            sizeof(semdata_t *) * (semCollection->semaphoresCapacity + BLOCK_SIZE));

        if (new == NULL) {
            return -1;
        }

        semCollection->semaphores = new;

        firstFree = semCollection->semaphoresCapacity;
        semCollection->semaphoresCapacity += BLOCK_SIZE;
    }

    // Create the semaphore
    semdata_t *semData = calloc(1, sizeof(semdata_t));

    if (semData == NULL) {
        return -1;
    }

    semData->name = name;
    semData->isLinked = true;

    semData->value = initialValue;

    semData->attachedProcesses = 1;

    semData->waitingQueue = newQueue(sizeof(pid_t), comparePid);
    semData->waitingQueueSize = 0;

    // SemId
    semData->sem = firstFree;

    // Add the semaphore to the collection
    semCollection->semaphores[firstFree] = semData;

    return semData->sem;
};

// Get unnamed semaphore
sem_t initUnnamedSem(semCollectionADT semCollection, uint64_t value) {

    if (semCollection == NULL) {
        return -1;
    }

    // Find first free position
    int firstFree = -1;
    for (int i = 0; i < semCollection->semaphoresCapacity; i++) {

        if (semCollection->semaphores[i] == NULL) {

            // First free position
            firstFree = i;
            break;
        }
    }

    // No free position
    if (firstFree == -1) {
        semdata_t **new = realloc(
            semCollection->semaphores,
            sizeof(semdata_t *) * (semCollection->semaphoresCapacity + BLOCK_SIZE));

        if (new == NULL) {
            return -1;
        }

        semCollection->semaphores = new;

        firstFree = semCollection->semaphoresCapacity;
        semCollection->semaphoresCapacity += BLOCK_SIZE;
    }

    // Create the semaphore
    semdata_t *semData = calloc(1, sizeof(semdata_t));

    if (semData == NULL) {
        return -1;
    }

    semData->value = value;

    semData->waitingQueue = newQueue(sizeof(pid_t), comparePid);
    semData->waitingQueueSize = 0;

    // SemId
    semData->sem = firstFree;

    // Add the semaphore to the collection
    semCollection->semaphores[firstFree] = semData;

    return semData->sem;
}

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

// Returns a copy of the waiting queue
pid_t * getWaitingQueue(semCollectionADT semCollection, sem_t sem, uint64_t *size) {

    // Check if the semaphore exists
    if (!semExists(semCollection, sem)) {
        return NULL;
    }

    // Get the queue size
    *size = semCollection->semaphores[sem]->waitingQueueSize;

    // Create a copy of the queue
    pid_t *queue = calloc(*size, sizeof(pid_t));

    if (queue == NULL) {
        *size = 0;
        return NULL;
    }

    pid_t *queueIter = queue;
    
    toBegin(semCollection->semaphores[sem]->waitingQueue);
    pid_t pid;
    while (hasNext(semCollection->semaphores[sem]->waitingQueue)) {
        next(semCollection->semaphores[sem]->waitingQueue, &pid);
        *queueIter = pid;
        queueIter++;
    }

    return queue;
}
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
int addWaitingProcess(semCollectionADT semCollection, sem_t sem, pid_t pid) {

    // Check if the semaphore exists
    if (!semExists(semCollection, sem)) {
        return -1;
    }

    queue(semCollection->semaphores[sem]->waitingQueue, &pid);
    semCollection->semaphores[sem]->waitingQueueSize++;

    return 0;
};

// Get the next process in the waiting queue
pid_t getNextWaitingProcess(semCollectionADT semCollection, sem_t sem) {

    // Check if the semaphore exists
    if (!semExists(semCollection, sem)) {
        return -1;
    }

    // Validate waiting queue not empty
    if (semCollection->semaphores[sem]->waitingQueueSize == 0) {
        return -1;
    }

    pid_t pid;
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

    // If the semaphore is unnamed, return -1
    if (!semCollection->semaphores[sem]->isLinked) {
        return -1;
    }

    // Decrement the attached processes
    semCollection->semaphores[sem]->attachedProcesses--;

    // If there are no more attached processes and the semaphore is not linked,
    // delete it
    if (semCollection->semaphores[sem]->attachedProcesses == 0 &&
        !semCollection->semaphores[sem]->isLinked) {

        // Free the semaphore
        freeQueue(semCollection->semaphores[sem]->waitingQueue);
        free(semCollection->semaphores[sem]);

        // Remove the semaphore from the collection
        semCollection->semaphores[sem] = NULL;
    }

    return 0;
};

// Unlink a semaphore
int64_t unlinkSem(semCollectionADT semCollection, const char *name) {

    if (semCollection == NULL || name == NULL) {
        return -1;
    }

    // Search for the semaphore
    int sem = -1;
    for (int i = 0; i < semCollection->semaphoresCapacity; i++) {
        // Semaphore found
        if (semCollection->semaphores[i] != NULL &&
            semCollection->semaphores[i]->isLinked &&
            strcmp(semCollection->semaphores[i]->name, name) == 0) {
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
        freeQueue(semCollection->semaphores[sem]->waitingQueue);
        free(semCollection->semaphores[sem]);

        // Remove the semaphore from the collection
        semCollection->semaphores[sem] = NULL;
    }

    return 0;
};

// Destroy an unnamed semaphore
int64_t destroyUnnamedSem(semCollectionADT semCollection, sem_t sem) {

    // If sem is not valid return -1
    if (!semExists(semCollection, sem)) {
        return -1;
    }

    // Free the semaphore
    freeQueue(semCollection->semaphores[sem]->waitingQueue);
    free(semCollection->semaphores[sem]);

    // Remove the semaphore from the collection
    semCollection->semaphores[sem] = NULL;

    return 0;
};

// ------------------- Free -------------------
// Free the semaphore collection
void freeSemCollection(semCollectionADT semCollection) {

    // Free the semaphores
    for (int i = 0; i < semCollection->semaphoresCapacity; i++) {
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

    if (semCollection == NULL) {
        return NULL;
    }

    // Create the array
    TSemInfo *res =
        calloc(1, sizeof(TSemInfo) * semCollection->semaphoresCapacity);

    if (res == NULL) {
        *size = 0;
        return NULL;
    }

    // Fill the array
    int i = 0;
    for (int j = 0; j < semCollection->semaphoresCapacity; j++) {
        if (semCollection->semaphores[j] != NULL) {

            // Get semaphore data
            res[i].value = semCollection->semaphores[j]->value;
            res[i].name = semCollection->semaphores[j]->name;
            res[i].waitingQueueSize = semCollection->semaphores[j]->waitingQueueSize;
            res[i].waitingQueue = NULL;

            if (semCollection->semaphores[j]->waitingQueueSize > 0) {

                // Get Blocked processes
                int *blockedProcesses = calloc(1, sizeof(int) * res[i].waitingQueueSize);

                if (blockedProcesses == NULL) {
                    *size = 0;
                    for (int k = 0; k < i; k++) {
                        free(res[k].waitingQueue);
                    }
                    free(res);
                    return NULL;
                }

                // Iterate the queue
                toBegin(semCollection->semaphores[j]->waitingQueue);
                int k = 0;
                while (hasNext(semCollection->semaphores[j]->waitingQueue)) {
                    next(semCollection->semaphores[j]->waitingQueue,
                         &blockedProcesses[k]);
                    k++;
                }

                res[i].waitingQueue = blockedProcesses;
            }

            i++;
        }
    }

    // Realloc the array
    TSemInfo *new = realloc(res, sizeof(TSemInfo) * i);

    if (new == NULL) {
        *size = 0;
        for (int k = 0; k < i; k++) {
            free(res[k].waitingQueue);
        }
        free(res);
        return NULL;
    }

    res = new;

    *size = i;
    return res;
};
