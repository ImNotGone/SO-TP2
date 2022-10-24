#include <libs/semaphore.h>

void acquire(int *lock){
  while(_xchg(lock, 1) != 0);
}

void release(int *lock){
  _xchg(lock, 0);
}

//
static uint8_t globalMutex = 0;

sem_t sem_open(const char * name, uint8_t value) {

}

int8_t sem_wait(sem_t * sem) {
    acquire(&globalMutex);

    release(&globalMutex);
}

int8_t sem_post(sem_t * sem) {
    acquire(&globalMutex);

    release(&globalMutex);
}

int8_t sem_close(sem_t * sem);


