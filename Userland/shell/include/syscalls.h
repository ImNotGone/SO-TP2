#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include <stdint.h>

// ------------ Data structures for kernel interaction -----------
// RTC time
typedef struct time {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} TTime;

// Inforegs registers
typedef struct regs {
    int64_t rax;
    int64_t rbx;
    int64_t rcx;
    int64_t rdx;
    int64_t rsi;
    int64_t rdi;
    int64_t rbp;
    int64_t rsp;
    int64_t r8;
    int64_t r9;
    int64_t r10;
    int64_t r11;
    int64_t r12;
    int64_t r13;
    int64_t r14;
    int64_t r15;
    int64_t rip;
} TRegs;

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

// sem_t
typedef int64_t sem_t;

typedef int16_t pid_t;

// Writes the amount of bytes from the fd into buff
extern int64_t sysread(uint64_t fd, char * buff, int64_t bytes);

// Writes the amount of bytes from the buff into the fd
extern int64_t syswrite(uint64_t fd, const char * buff, int64_t bytes);

// Gets time in TTime format from the rtc
extern void systime(TTime * ts);

// Writes the MEM_DUMP_COUNT bytes from address into memData
extern int64_t sysmemdump(uint64_t address, int8_t *memData);

// Gets the registeres in TRegs format from the kernel register snapshot
extern int8_t sysregdump(TRegs *regs);

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
extern void sysmeminfo(TMemInfo *memInfo);

//returns pid
extern pid_t syscreateprocess(uint64_t rip, int ground, int priority, int argc, char * argv[] );

//exits process
extern void sysexits();

//executes process
extern void sysexec();

//prints out all process' data
extern void sysps();

// Kills the process with the pid
extern int64_t syskill(pid_t pid);

// Waits for the process with the pid to finish
extern int64_t syswaitpid(pid_t pid);

// Changes the priority of the process
extern void sysnice(pid_t pid, int priority);

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

// Destroys unnamed semaphore
extern int64_t sysdup(pid_t pid, uint64_t prev, uint64_t new );


#endif//SYSCALLS_H_
