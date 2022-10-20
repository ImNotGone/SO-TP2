#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>

#define MAX_MEM_ADDRESS 0xFFFFfffff
#define MEM_DUMP_SIZE 32
#define STDIN   0
#define STDOUT  1
#define STDERR  2


int64_t syscallDispatcher(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8);

void saveRegs(const int64_t * regs);

#endif//SYSCALLS_H
