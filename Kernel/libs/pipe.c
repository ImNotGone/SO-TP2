// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "libs/string.h"
#include <libs/memoryManager.h>
#include <ADTS/hashmapADT.h>
#include <libs/semaphore.h>

#define PIPE_SIZE 512
#define READ 0
#define WRITE 1
#define EOF -1

typedef struct pipe {
    int8_t * buffer;
    uint64_t size; // TODO: remove if resize will not be implemented
    uint64_t readerOff;
    uint64_t writerOff;
    sem_t readerSem, writerSem, mutex;

    int8_t readerClosed;
    int8_t writerClosed;
} pipe_t;


typedef struct node {
    char * name;
    uint64_t pipeId;
    struct node * tail;
} tNode;

typedef enum {MEMERROR, ADDED, EXISTED} tFlag;

typedef tNode * tList;
static tList NPList = NULL;
static tList addNamedPipe(tList first, char * name, uint64_t * pipeId, tFlag * flag);
static tList removeNamedPipe(tList first, char * name);
static bool findNamedPipe(tList first, char * name, uint64_t * pipeId);

static hashMapADT pipeMap = NULL;
static uint64_t pipeId = 2; // starts at 2 so that the first pipe is 4/5 second 6/7 etc...

int64_t pipe(fd_t pipefd[2]) {
    if(pipeMap == NULL) {
        pipeMap = newHashMap(sizeof(uint64_t), sizeof(pipe_t *), hashInt);
    }
    pipe_t * new = malloc(sizeof(pipe_t));

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
    new->readerClosed = 0;
    new->writerClosed = 0;
    new->readerSem = sem_init(1);
    new->writerSem = sem_init(0);
    new->mutex     = sem_init(1);

    putHm(pipeMap, &pipeId, &new);
    pipefd[READ] = pipeId * 2;
    pipefd[WRITE] = pipeId * 2 + 1;
    pipeId++;
    return 1;
}

int64_t namedPipe(char * name, fd_t pipefd[2]) {

    uint64_t pipeId;
    pipe_t * aux;
    if(pipeMap != NULL && findNamedPipe(NPList, name, &pipeId) && findHm(pipeMap, &pipeId, &aux)) {
        pipefd[READ] = pipeId*2;
        pipefd[WRITE] = pipeId*2+1;
        return 1;
    }

    if(pipe(pipefd) == -1) {
        return -1;
    }
    tFlag flag;
    pipeId = pipefd[READ]/2;
    NPList = addNamedPipe(NPList, name, &pipeId, &flag);
    if(flag == MEMERROR || flag == EXISTED /* should not happen */) {
        return -1;
    }
    return 1;
}

int64_t pipeRead(fd_t fd, char * buffer, uint64_t bytes) {
    if(pipeMap == NULL || buffer == NULL || bytes == 0 || fd < 4) {
        return -1;
    }

    // All reader fds are even
    if(fd % 2 != 0) {
        return -1;
    }
    // "get pipeId"
    uint64_t pipeId = fd/2;

    pipe_t * pipe;
    if(!findHm(pipeMap, &pipeId, &pipe)) {
        return -1;
    }

    int64_t i = 0;
    if(sem_wait(pipe->readerSem) == -1) {
        return -1;
    }
    while(i < bytes) {
        if(sem_wait(pipe->writerSem) == -1) {
            return -1;
        }

        buffer[i] = pipe->buffer[pipe->readerOff++];
        pipe->readerOff = pipe->readerOff % pipe->size;

        if ((int) buffer[i] == EOF) {
            break;
        }

        i++;
    }
    if(sem_post(pipe->readerSem) == -1) {
        return -1;
    }
    return i;
}

int64_t pipeWrite(fd_t fd, const char * buffer, uint64_t bytes) {
    if(pipeMap == NULL || buffer == NULL || bytes == 0 || fd < 4) {
        return -1;
    }

    // All writer fds are uneven
    if(fd % 2 == 0) {
        return -1;
    }
    // "get pipeId"
    uint64_t pipeId = fd/2;

    pipe_t * pipe;
    if(!findHm(pipeMap, &pipeId, &pipe)) {
        return -1;
    }

    int64_t i = 0;
    if(sem_wait(pipe->mutex) == -1) {
        return -1;
    }
    while(i < bytes) {

        pipe->buffer[pipe->writerOff++] = buffer[i];
        pipe->writerOff = pipe->writerOff % pipe->size;
        i++;

        if(sem_post(pipe->writerSem) == -1) {
            return -1;
        }
    }
    if(sem_post(pipe->mutex) == -1) {
        return -1;
    }



    return i;
}

static int64_t signalEOF(fd_t fd) {
    if(pipeMap == NULL || fd < 4) {
        return -1;
    }

    // All writer fds are uneven
    if(fd % 2 == 0) {
        return -1;
    }
    // "get pipeId"
    uint64_t pipeId = fd/2;

    pipe_t * pipe;
    if(!findHm(pipeMap, &pipeId, &pipe)) {
        return -1;
    }

    if(sem_wait(pipe->mutex) == -1) {
        return -1;
    }

    pipe->buffer[pipe->writerOff++] = EOF;
    pipe->writerOff = pipe->writerOff % pipe->size;

    if(sem_post(pipe->writerSem) == -1) {
        return -1;
    }
    if(sem_post(pipe->mutex) == -1) {
        return -1;
    }
    return 1;
}

int64_t pipeClose(fd_t fd) {
    if(fd < 4) {
        return -1;
    }
    // "get pipeId"
    uint64_t pipeId = fd/2;
    pipe_t * pipe;
    if(!findHm(pipeMap, &pipeId, &pipe)) {
        return -1;
    }


    // If is writer signal EOF
    if(fd % 2 != 0) {
        if(signalEOF(fd) == -1) {
            return -1;
        }
    }

    // If the other fd is closed, free the pipe, else just close this fd
    if((fd % 2 == 0 && pipe->writerClosed) || (fd % 2 != 0 && pipe->readerClosed)) {
        sem_destroy(pipe->readerSem);
        sem_destroy(pipe->writerSem);
        sem_destroy(pipe->mutex);
        free(pipe->buffer);
        free(pipe);
        removeHm(pipeMap, &pipeId);
    } else {
        if(fd % 2 == 0) {
            pipe->readerClosed = 1;
        } else {
            pipe->writerClosed = 1;
        }
    }

    return 1;
}

// ====================== AUX =========================

static tList addNamedPipe(tList first, char * name, uint64_t * pipeId, tFlag * flag) {
    int c = 0;
    if(first == NULL || (c = strcmp(first->name, name)) > 0) {
        tList new = malloc(sizeof(tNode));
        new->name = malloc(strlen(name) + 1);
        if(new == NULL || new->name == NULL) {
            free(new);
            *flag = MEMERROR;
            return first;
        }
        new->pipeId = *pipeId;
        new->tail = first;
        *flag = ADDED;
        return new;
    }
    if(c == 0) {
        *flag = EXISTED;
        *pipeId = first->pipeId;
        return first;
    }
    first->tail = addNamedPipe(first->tail, name, pipeId, flag);
    return first;
}

static tList removeNamedPipe(tList first, char * name) {
    int c = 0;
    if(first == NULL || (c = strcmp(first->name, name)) > 0) {
        return first;
    }
    if(c == 0) {
        tList aux = first->tail;
        free(first->name);
        free(first);
        return aux;
    }
    first->tail = removeNamedPipe(first->tail, name);
    return first;
}

static bool findNamedPipe(tList first, char * name, uint64_t * pipeId) {
    int c = 0;
    if(first == NULL || (c = strcmp(first->name, name)) > 0) {
        return false;
    }
    if(c == 0) {
        *pipeId = first->pipeId;
        return true;
    }
    return findNamedPipe(first->tail, name, pipeId);
}
