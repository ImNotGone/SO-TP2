#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <types.h>
#include <ADTS/queueADT.h>
#include <stdint.h>
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

    // Waiting Processes
    queueADT waiting_processes;
}PCBType;

//TODO work here and in scheduler to remove this struct
//the scheduler should only take PCBType * pcb


// C functions

pid_t newProcess(uint64_t rip, int ground, int priority, int argc, char * argv[]);

void exec(pid_t pid);

int64_t killProcess(pid_t pid);

void changePriority(pid_t pid, int priority);

int64_t unblockProcess(pid_t pid);

int64_t blockProcess(pid_t pid);

int64_t blockProcessOnInput(pid_t pid);

int64_t waitProcess(pid_t pid);

int dup(pid_t pid, fd_t prev, fd_t new);

int64_t sleepProcess(pid_t pid, uint64_t seconds);

void printAllProcess();

int64_t comparePCB(void * pcb1, void * pcb2);

int64_t fork(uint64_t rip, uint64_t rsp);

#endif //PROCESSMANAGER_H

