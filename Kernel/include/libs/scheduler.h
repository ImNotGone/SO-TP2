#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <libs/string.h>
#include <libs/processManager.h>
#include <ADTS/queueADT.h>

void startScheduler();

uint64_t switchContext(uint64_t rsp);

void addToReadyQueue(Pdata process);

int getActivePid();

void yield();

#endif //SCHEDULER_H
