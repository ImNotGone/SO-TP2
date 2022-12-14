#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <types.h>
#include <ADTS/semCollectionADT.h>
#include <libs/processManager.h>
#include <libs/scheduler.h>

extern int _xadd(int inc, int * value);
extern int _xchg(int * lock, int value);

void initSemaphores();

void freeSemaphores();

sem_t sem_open(const char * name, uint32_t value);

int64_t sem_wait(sem_t sem);

int64_t sem_post(sem_t sem);

int64_t sem_close(sem_t sem);

int64_t sem_unlink(const char *name);

sem_t sem_init(uint64_t value);

int64_t sem_destroy(sem_t sem);

TSemInfo *sem_info(uint64_t * size);

pid_t *sem_waiting_queue(sem_t sem, uint64_t *size);
#endif // _SEMAPHORE_H
