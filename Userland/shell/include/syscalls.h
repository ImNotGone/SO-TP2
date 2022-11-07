#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include <stdint.h>

// ------------ Data structures for kernel interaction -----------

// sem_t
typedef int64_t sem_t;

typedef int64_t pid_t;

typedef int fd_t;

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

// Pipe Info structure
typedef struct pipeinfo {
    const char *name;
    uint64_t size;

    uint64_t readerOffset;
    uint64_t writerOffset;

    pid_t *waitingProcesses;
    uint64_t waitingProcessesSize;
} TPipeInfo;


// Writes the amount of bytes from the fd into buff
extern int64_t sysread(uint64_t fd, char * buff, int64_t bytes);

// Writes the amount of bytes from the buff into the fd
extern int64_t syswrite(uint64_t fd, const char * buff, int64_t bytes);

// Allocates memory with te rewuired size
extern void *sysmalloc(uint64_t size);

// Frees the memory allocated with sysmalloc
extern void sysfree(void *ptr);

// Reallocation of memory
// If ptr is NULL, it allocates a new memory block
extern void *sysrealloc(void *ptr, uint64_t size);

// Calloc
extern void *syscalloc(uint64_t nmemb, uint64_t size);

// Gets memory manager information
extern int64_t sysmeminfo(TMemInfo *memInfo);

//returns pid
extern pid_t syscreateprocess(uint64_t rip, int ground, int priority, int argc, char * argv[] );

//exits process
extern void sysexits();

//executes process
extern void sysexec();

//prints out all process' data
extern TProcInfo *sysps(uint64_t *size);

// Kills the process with the pid
extern int64_t syskill(pid_t pid);

// Waits for the process with the pid to finish
extern int64_t syswaitpid(pid_t pid);

// Changes the priority of the process
extern int64_t sysnice(pid_t pid, int priority);

// Blocks the process
extern int64_t sysblock(pid_t pid);

// Unblocks the process
extern int64_t sysunblock(pid_t pid);


extern int64_t syssleep(pid_t pid, int64_t seconds);

// Gives up the CPU
extern void sysyield();

// Gets the current process id
extern uint64_t sysgetpid();

// Opens a semaphore
extern sem_t syssemopen(const char * name, uint64_t value);

// Waits for a semaphore
extern int64_t syssemwait(sem_t sem);

// Posts a semaphore
extern int64_t syssempost(sem_t sem);

// Closes a semaphore
extern int64_t syssemclose(sem_t sem);

// Unlinks a semaphore
extern int64_t syssemunlink(const char * name);

// Gets semaphore information
extern TSemInfo *sysseminfo(uint64_t * size);

// Initializes unnamed semaphore
extern sem_t sysseminit(uint64_t value);

// Destroys unnamed semaphore
extern int64_t syssemdestroy(sem_t sem);

// Changes file descriptor
extern int64_t sysdup(pid_t pid, uint64_t prev, uint64_t new );

// Creates a pipe
extern int64_t syspipe(fd_t fds[2]);

// Closes a file descriptor
extern int64_t sysclose(fd_t fd);

// Creates a named pipe
extern int64_t sysmkfifo(const char * name, fd_t fds[2]);

// Unlinks a named pipe
extern int64_t sysunlink(const char * name);

// Gets pipe information
extern TPipeInfo *syspipeinfo(uint64_t * size);

#endif//SYSCALLS_H_
