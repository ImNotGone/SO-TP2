// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libs/memoryManager.h>
#include <ADTS/hashmapADT.h>
#include <libs/semaphore.h>

#define PIPE_SIZE 512
#define READ 0
#define WRITE 1

typedef struct pipe {
    int8_t * buffer;
    uint64_t size; // TODO: remove if resize will not be implemented
    uint64_t readerOff;
    uint64_t writerOff;
    sem_t readerSem, writerSem, mutex;
} pipe_t;

static hashMapADT pipeMap = NULL;
static uint64_t pipeId = 2; // starts at 2 so that the first pipe is 4/5 second 6/7 etc...

int64_t pipe(fd_t pipefd[2]) {
    if(pipeMap == NULL) {
        pipeMap = newHashMap(sizeof(uint64_t), sizeof(pipe_t *), hashInt);
    }
    pipe_t * new = malloc(sizeof(pipe_t));
    new->buffer = malloc(PIPE_SIZE * sizeof(int8_t));
    if(new == NULL || new->buffer == NULL) {
        free(new);
        return -1;
    }
    new->size = PIPE_SIZE;
    new->readerOff = 0;
    new->writerOff = 0;
    new->readerSem = sem_init(1);
    new->writerSem = sem_init(0);
    new->mutex     = sem_init(1);

    putHm(pipeMap, &pipeId, &new);
    pipefd[READ] = pipeId * 2;
    pipefd[WRITE] = pipeId * 2 + 1;
    pipeId++;
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

    // TODO: check if other process are using the pipe
    // if(being used) {
    //      return 1;
    // }

    sem_destroy(pipe->readerSem);
    sem_destroy(pipe->writerSem);
    free(pipe->buffer);
    free(pipe);
    return 1;
}

