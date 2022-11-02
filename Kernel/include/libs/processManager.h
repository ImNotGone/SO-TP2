#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <stdint.h>
#include <libs/process.h>
#include <interrupts/interrupts.h>
#include <drivers/graphics.h>

#define READY 0
#define BLOCKED 1
#define KILLED 2
#define STACK_SIZE 4096

typedef struct processData{
    uint16_t pid, ppid;
    uint64_t rsp, stack_base, rip;
    int argc;
    int ground;
    int priority;
    int status;
    char ** argv;
    char * name;
}PCBType;

//TODO work here and in scheduler to remove this struct
//the scheduler should only take PCBType * pcb


// C functions

uint64_t newProcess(uint64_t rip, int ground, int priority, int argc, char * argv[]);

void exec(uint64_t pid);

void killProcess();

void changePriority(int pid, int priority);

void unblock(uint64_t pid);

void block(uint64_t pid);

void printAllProcess();

int isForeground(int pid);

#endif //PROCESSMANAGER_H

