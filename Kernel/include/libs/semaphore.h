#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <types.h>

sem_t sem_open(const char * name, uint8_t value);

int8_t sem_wait(sem_t * sem);

int8_t sem_post(sem_t * sem);

int8_t sem_close(sem_t * sem);

#endif // _SEMAPHORE_H
