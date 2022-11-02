#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>

#define false 0
#define true !false

typedef int8_t bool;

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

// sem_t
typedef int64_t sem_t;

// lock_t
typedef int lock_t;

#endif//_TYPES_H
