// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libs/semaphore.h>

typedef int lock_t;

void acquire(lock_t *lock){
  while(_xchg(lock, 1) != 0);
}

void release(lock_t *lock){
  _xchg(lock, 0);
}

static lock_t globalMutex = 0;

sem_t sem_open(const char * name, uint32_t value) {

    // TODO: check if resource name is available
    //if(!nameAvailable) {
    //  return -1;
    //}

    sem_t out;
    out.value = value;
    return out;
}

int64_t sem_wait(sem_t * sem) {
    acquire(&globalMutex);

    release(&globalMutex);
}

int64_t sem_post(sem_t * sem) {
    acquire(&globalMutex);

    release(&globalMutex);
}

int64_t sem_close(sem_t * sem);


