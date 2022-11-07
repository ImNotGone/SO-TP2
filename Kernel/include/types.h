#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>

#define false 0
#define true !false

#define STDIN   0
#define STDOUT  1
#define STDERR  2

typedef int8_t bool;

// ------------ Data structures for kernel interaction -----------
// sem_t
typedef int64_t sem_t;

// lock_t
typedef int lock_t;

typedef int16_t pid_t;

typedef int fd_t;
//
// MemInfo structure defined in memoryManager.h
typedef struct meminfo {
    uint64_t total;
    uint64_t free;
    uint64_t used;
} TMemInfo;

// Semaphores Info structure
typedef struct seminfo {
    uint64_t value;
    const char *name;

    uint64_t waitingQueueSize;
    int *waitingQueue;
} TSemInfo;

// Process Info structure
typedef struct procinfo {
    pid_t pid;

    char *name;

    int priority;

    uint64_t stackBase, rsp;

    int ground;
    int status;

    pid_t *waitingProcesses;
    uint64_t waitingProcessesSize;

} TProcInfo;

#endif//_TYPES_H
