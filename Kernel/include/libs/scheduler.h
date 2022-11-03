#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <libs/string.h>
#include <libs/processManager.h>
#include <ADTS/queueADT.h>


void startScheduler();

queueADT getQueue();

void printPs();

PCBType * find(pid_t pid);

uint64_t switchContext(uint64_t rsp);

void addToReadyQueue(PCBType ** process);

pid_t getActivePid();

void yield();

#endif //SCHEDULER_H
