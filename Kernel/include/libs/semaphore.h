#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <types.h>

extern int64_t _xadd(int64_t inc, uint64_t * value);
extern int64_t _xchg(uint64_t * lock, int64_t value);

sem_t sem_open(const char * name, uint8_t value);

int8_t sem_wait(sem_t * sem);

int8_t sem_post(sem_t * sem);

int8_t sem_close(sem_t * sem);

#endif // _SEMAPHORE_H
