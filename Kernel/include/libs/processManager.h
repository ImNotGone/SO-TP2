#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <stdint.h>
#include <libs/process.h>
#include <interrupts/interrupts.h>
#include <drivers/graphics.h>


typedef struct processData{
    uint16_t pid, ppid;
    uint64_t rsp, stack_base, rip;
    int argc;
    int ground;
    int priority;
    char * status;
    char ** argv;
    char * name;
}PCBType;

//TODO work here and in scheduler to remove this struct
//the scheduler should only take PCBType * pcb
typedef struct pdata{
    PCBType * pcb;
    int pid;
}Pdata;

// C functions
uint64_t newProcess(uint64_t rip, int ground, int priority, int argc, char * argv[]);

void exec(uint64_t pid);

void killProcess();

void changePriority(uint64_t pid, uint64_t priority);

void unblock(uint64_t pid);

void block(uint64_t pid);

void printAllProcess();

#endif //PROCESSMANAGER_H

