// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "libs/string.h"
#include <ADTS/hashmapADT.h>
#include <libs/memoryManager.h>
#include <libs/semaphore.h>

#define PIPE_SIZE 512
#define READ 0
#define WRITE 1
#define EOF -1

typedef struct pipe {
    int8_t *buffer;
    uint64_t size;
    uint64_t readerOff;
    uint64_t writerOff;
    sem_t readerSem, writerSem, mutex;

    uint64_t readers;
    uint64_t writers;

} pipe_t;

typedef struct node {
    char *name;
    uint64_t pipeId;
    struct node *tail;
} tNode;

typedef enum { MEMERROR,
               ADDED,
               EXISTED } tFlag;

typedef tNode *tList;
static tList NPList = NULL;
static tList addNamedPipe(tList first, const char *name, uint64_t *pipeId,
                          tFlag *flag);
static tList removeNamedPipe(tList first, const char *name);
static bool findNamedPipe(tList first, const char *name, uint64_t *pipeId);
static void freePipe(pipe_t *pipe);
static const char * getPipeName(tList first, uint64_t pipeId);

static hashMapADT pipeMap = NULL;
static uint64_t pipeId = 2; // starts at 2 so that the first pipe is 4/5 second 6/7 etc...

int64_t pipe(fd_t pipefd[2]) {
    if (pipeMap == NULL) {
        pipeMap = newHashMap(sizeof(uint64_t), sizeof(pipe_t *), hashInt);
    }
    pipe_t *new = malloc(sizeof(pipe_t));

    if (new == NULL) {
        return -1;
    }

    new->buffer = malloc(PIPE_SIZE * sizeof(int8_t));
    if (new->buffer == NULL) {
        free(new);
        return -1;
    }

    new->size = PIPE_SIZE;
    new->readerOff = 0;
    new->writerOff = 0;
    new->readerSem = sem_init(1);
    new->writerSem = sem_init(0);
    new->mutex = sem_init(1);
    new->readers = 1;
    new->writers = 1;

    putHm(pipeMap, &pipeId, &new);
    pipefd[READ] = pipeId * 2;
    pipefd[WRITE] = pipeId * 2 + 1;
    pipeId++;
    return 0;
}

int64_t namedPipe(const char *name, fd_t pipefd[2]) {

    uint64_t pipeId;
    pipe_t *aux;
    if (pipeMap != NULL && findNamedPipe(NPList, name, &pipeId) &&
        findHm(pipeMap, &pipeId, &aux)) {
        pipefd[READ] = pipeId * 2;
        pipefd[WRITE] = pipeId * 2 + 1;

        aux->readers++;
        aux->writers++;

        return 0;
    }

    if (pipe(pipefd) == -1) {
        return -1;
    }
    tFlag flag;
    pipeId = pipefd[READ] / 2;
    NPList = addNamedPipe(NPList, name, &pipeId, &flag);
    if (flag == MEMERROR || flag == EXISTED /* should not happen */) {
        return -1;
    }
    return 0;
}

int64_t unlinkpipe(const char *name) {
    uint64_t pipeId;
    if (!findNamedPipe(NPList, name, &pipeId)) {
        return -1;
    }
    NPList = removeNamedPipe(NPList, name);

    // Get pipe
    pipe_t *pipe;
    if (!findHm(pipeMap, &pipeId, &pipe)) {
        return -1;
    }

    // If both ends of the pipe are closed, free the pipe
    if (pipe->readers == 0 && pipe->writers == 0) {
        freePipe(pipe);
        removeHm(pipeMap, &pipeId);
    }

    return 0;
}

// Duplicate the fd
int64_t addToPipe(fd_t fd) {
    uint64_t pipeId = fd / 2;
    pipe_t *pipe;
    if (!findHm(pipeMap, &pipeId, &pipe)) {
        return -1;
    }

    if (fd % 2 == 0) {
        pipe->readers++;
    } else {
        pipe->writers++;
    }

    return fd;
}


int64_t pipeRead(fd_t fd, char *buffer, uint64_t bytes) {
    if (pipeMap == NULL || buffer == NULL || bytes == 0 || fd < 4) {
        return -1;
    }

    // All reader fds are even
    if (fd % 2 != 0) {
        return -1;
    }
    // "get pipeId"
    uint64_t pipeId = fd / 2;

    pipe_t *pipe;
    if (!findHm(pipeMap, &pipeId, &pipe)) {
        return -1;
    }

    int64_t i = 0;
    if (sem_wait(pipe->readerSem) == -1) {
        return -1;
    }
    while (i < bytes) {
        if (sem_wait(pipe->writerSem) == -1) {
            return -1;
        }

        buffer[i] = pipe->buffer[pipe->readerOff++];
        pipe->readerOff = pipe->readerOff % pipe->size;

        if ((int)buffer[i] == EOF) {
            break;
        }

        i++;
    }
    if (sem_post(pipe->readerSem) == -1) {
        return -1;
    }
    return i;
}

int64_t pipeWrite(fd_t fd, const char *buffer, uint64_t bytes) {
    if (pipeMap == NULL || buffer == NULL || bytes == 0 || fd < 4) {
        return -1;
    }

    // All writer fds are uneven
    if (fd % 2 == 0) {
        return -1;
    }
    // "get pipeId"
    uint64_t pipeId = fd / 2;

    pipe_t *pipe;
    if (!findHm(pipeMap, &pipeId, &pipe)) {
        return -1;
    }

    int64_t i = 0;
    if (sem_wait(pipe->mutex) == -1) {
        return -1;
    }
    while (i < bytes) {

        pipe->buffer[pipe->writerOff++] = buffer[i];
        pipe->writerOff = pipe->writerOff % pipe->size;
        i++;

        if (sem_post(pipe->writerSem) == -1) {
            return -1;
        }
    }
    if (sem_post(pipe->mutex) == -1) {
        return -1;
    }

    return i;
}

static int64_t signalEOF(fd_t fd) {
    if (pipeMap == NULL || fd < 4) {
        return -1;
    }

    // All writer fds are uneven
    if (fd % 2 == 0) {
        return -1;
    }
    // "get pipeId"
    uint64_t pipeId = fd / 2;

    pipe_t *pipe;
    if (!findHm(pipeMap, &pipeId, &pipe)) {
        return -1;
    }

    if (sem_wait(pipe->mutex) == -1) {
        return -1;
    }

    pipe->buffer[pipe->writerOff++] = EOF;
    pipe->writerOff = pipe->writerOff % pipe->size;

    if (sem_post(pipe->writerSem) == -1) {
        return -1;
    }
    if (sem_post(pipe->mutex) == -1) {
        return -1;
    }
    return 0;
}

int64_t pipeClose(fd_t fd) {
    if (fd < 4) {
        return -1;
    }
    // "get pipeId"
    uint64_t pipeId = fd / 2;
    pipe_t *pipe;
    if (!findHm(pipeMap, &pipeId, &pipe)) {
        return -1;
    }

    // If is writer signal EOF
    if (fd % 2 != 0 && pipe->writers == 1) {
        if (signalEOF(fd) == -1) {
            return -1;
        }
    }

    if (fd % 2 == 0) {
        pipe->readers--;
    } else {
        pipe->writers--;
    }

    // Get pipe name
    const char *pipeName = getPipeName(NPList, pipeId);

    // If there are no more processes and the pipe is not linked to a name , free the pipe
    if (pipe->readers == 0 && pipe->writers == 0 && !findNamedPipe(NPList, pipeName, &pipeId)) {

        freePipe(pipe);
        removeHm(pipeMap, &pipeId);
    }

    return 0;
}

// Pipes dump
TPipeInfo *pipeDump(uint64_t *size) {
    if (pipeMap == NULL) {
        *size = 0;
        return NULL;
    }

    // Get pipes
    uint64_t pipesize = 0;
    pipe_t ***pipes = (pipe_t ***) valuesHm(pipeMap, &pipesize);
    uint64_t **pipeIds = (uint64_t **) keysHm(pipeMap, &pipesize);


    // Allocate memory
    TPipeInfo *info = malloc(sizeof(TPipeInfo) * pipesize);
    if (info == NULL) {

        free(pipes);
        free(pipeIds);

        *size = 0;
        return NULL;
    }


    // Fill info
    for (uint64_t i = 0; i < pipesize; i++) {

        info[i].size = (*pipes[i])->size;
        info[i].readerOffset = (*pipes[i])->readerOff;
        info[i].writerOffset = (*pipes[i])->writerOff;

        // Get pipe name
        info[i].name = getPipeName(NPList, *pipeIds[i]);

        // Get waiting processes
        uint64_t waitingSize;
        info[i].waitingProcesses = sem_waiting_queue((*pipes[i])->writerSem, &waitingSize);
        info[i].waitingProcessesSize = waitingSize;
    }

    *size = pipesize;

    free(pipes);
    free(pipeIds);

    return info;
}


// ====================== AUX =========================

static tList addNamedPipe(tList first, const char *name, uint64_t *pipeId,
                          tFlag *flag) {
    int c = 0;
    if (first == NULL || (c = strcmp(first->name, name)) > 0) {
        tList new = malloc(sizeof(tNode));

        if (new == NULL) {
            *flag = MEMERROR;
            return first;
        }

        new->name = malloc(strlen((char *) name) + 1);
        if (new->name == NULL) {
            free(new);
            *flag = MEMERROR;
            return first;
        }

        strcpy(new->name, (char *) name);

        new->pipeId = *pipeId;
        new->tail = first;
        *flag = ADDED;
        return new;
    }
    if (c == 0) {
        *flag = EXISTED;
        *pipeId = first->pipeId;
        return first;
    }
    first->tail = addNamedPipe(first->tail, name, pipeId, flag);
    return first;
}

static tList removeNamedPipe(tList first, const char *name) {
    int c = 0;
    if (first == NULL || (c = strcmp(first->name, name)) > 0) {
        return first;
    }
    if (c == 0) {
        tList aux = first->tail;
        free(first->name);
        free(first);
        return aux;
    }
    first->tail = removeNamedPipe(first->tail, name);
    return first;
}

static bool findNamedPipe(tList first, const char *name, uint64_t *pipeId) {
    int c = 0;
    if (first == NULL || (c = strcmp(first->name, name)) > 0) {
        return false;
    }
    if (c == 0) {
        *pipeId = first->pipeId;
        return true;
    }
    return findNamedPipe(first->tail, name, pipeId);
}

static const char * getPipeName(tList first, uint64_t pipeId) {
    if (first == NULL) {
        return NULL;
    }
    if (first->pipeId == pipeId) {
        return first->name;
    }
    return getPipeName(first->tail, pipeId);
}

static void freePipe(pipe_t *pipe) {
    sem_destroy(pipe->readerSem);
    sem_destroy(pipe->writerSem);
    sem_destroy(pipe->mutex);
    free(pipe->buffer);
    free(pipe);
}
