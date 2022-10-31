// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libs/processManager.h>
#include <libs/scheduler.h> //dont move from here, necessary to avoid .h recursion


#define MAX_PROCESS 30
#define STACK_SIZE 4048
#define KERNEL_PID -1
#define MIN_PRIORITY 1
#define MAX_PRIORITY 5

static PCBType pcb[MAX_PROCESS]={0};
static int stack[MAX_PROCESS][STACK_SIZE]={0};
static int processCount = 0;

uint64_t newProcess(uint64_t rip, int ground, int priority, int argc, char * argv[]){

    pcb[processCount].stack_base = stack[processCount]+ STACK_SIZE;
    pcb[processCount].rip = rip;
    pcb[processCount].argc = argc;
    pcb[processCount].ground = ground;
    pcb[processCount].argv = argv;
    pcb[processCount].status= "ready";
    //pcb[processCount].name = name;
    pcb[processCount].priority=priority;
    pcb[processCount].pid = processCount;
    pcb[processCount].ppid = getActivePid();

    pcb[processCount].rsp = createProcess(pcb[processCount].stack_base, pcb[processCount].rip, argc, argv);

    return processCount++;
}

void exec(uint64_t pid){
    Pdata process = {&pcb[pid], pcb[pid].pid};
    addToReadyQueue(process);

    if(isForeground(pid)){
        block(pcb[pid].ppid);
    }
}

void killProcess(uint64_t pid){
    pcb[pid].status = "killed";

    if(isForeground(pid))
        unblock(pcb[pid].ppid);
}

void block(pid){
    pcb[pid].status="blocked";
    if (pid == getActivePid()){
        yield();
    }
}

void unblock(uint64_t pid){
    pcb[pid].status = "ready";
}

int isForeground(int pid){
    return !pcb[pid].ground;
}

