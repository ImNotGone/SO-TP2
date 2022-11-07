#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <libs/string.h>
#include <libs/processManager.h>
#include <ADTS/queueADT.h>


void startScheduler();

queueADT getQueue();

TProcInfo *procDump(uint64_t *size);

PCBType * getActiveProcess();

PCBType * findProcess(pid_t pid);

PCBType * removeProcess(pid_t pid);

uint64_t switchContext(uint64_t rsp);

void addToReadyQueue(PCBType ** process);

pid_t getActivePid();

void yield();

void wakeUpBlockedOnInput();

void setBlockedOnInput(PCBType * process);

void wakeUpProcesses();

void removeFromSleepingQueue(PCBType *process);

// Adds a process to the sleeping queue
void addToSleepingQueue(PCBType * process, uint64_t time);
#endif //SCHEDULER_H
