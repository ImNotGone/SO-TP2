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

// Gets memory manager information
extern void sysmeminfo(void *memInfo);

#endif//SYSCALLS_H_
