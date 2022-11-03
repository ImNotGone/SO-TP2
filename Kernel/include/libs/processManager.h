#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <types.h>
#include <libs/process.h>
#include <interrupts/interrupts.h>
#include <drivers/graphics.h>

#define READY 0
#define BLOCKED 1
#define KILLED 2
#define STACK_SIZE 4096

typedef struct processData{
    pid_t pid, ppid;
    uint64_t rsp, stack_base, rip;
    int argc;
    int ground;
    int priority;
    int status;
    char ** argv;
    char * name;
    int fd[3];
}PCBType;

//TODO work here and in scheduler to remove this struct
//the scheduler should only take PCBType * pcb


// C functions

pid_t newProcess(uint64_t rip, int ground, int priority, int argc, char * argv[]);

void exec(pid_t pid);

void killProcess(pid_t pid);

void changePriority(pid_t pid, int priority);

void unblock(pid_t pid);

void block(pid_t pid);

void printAllProcess();

int64_t comparePCB(void * pcb1, void * pcb2);

#endif //PROCESSMANAGER_H

