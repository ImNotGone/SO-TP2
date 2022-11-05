#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <types.h>
#include <libs/memoryManager.h>
#include <drivers/graphics.h>
#include <drivers/keyboard.h>
#include <drivers/RTC.h>
#include <libs/processManager.h>
#include <libs/scheduler.h>
#include <interrupts/interrupts.h>
#include <libs/semaphore.h>
#include <libs/pipe.h>

#define MAX_MEM_ADDRESS 0xFFFFfffff
#define MEM_DUMP_SIZE 32

int64_t syscallDispatcher(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8);

void saveRegs(const int64_t * regs);

#endif//SYSCALLS_H
