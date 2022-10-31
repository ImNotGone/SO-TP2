#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <stdint.h>
#include <libs/process.h>
#include <interrupts/interrupts.h>


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


typedef struct pdata{
    PCBType * pcb;
    int pid;
}Pdata;

// C functions
uint64_t newProcess(uint64_t rip, int ground, int priority, int argc, char * argv[]);

void exec(uint64_t pid);

void killProcess();

#endif //PROCESSMANAGER_H

