#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <types.h>

extern int _xadd(int inc, int * value);
extern int _xchg(int * lock, int value);

sem_t sem_open(const char * name, uint32_t value);

int64_t sem_wait(sem_t * sem);

int64_t sem_post(sem_t * sem);

int64_t sem_close(sem_t * sem);

#endif // _SEMAPHORE_H
