// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libs/processManager.h>
#include <libs/scheduler.h> //dont move from here, necessary to avoid .h recursion


#define MAX_PROCESS 30
#define STACK_SIZE 4096
#define KERNEL_PID -1
#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

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

void block(uint64_t pid){
    pcb[pid].status="blocked";
    if (pid == getActivePid()){
        yield();
    }
}

void unblock(uint64_t pid){
    pcb[pid].status = "ready";
}

void changePriority(uint64_t pid, uint64_t priority){
    if(priority>MAX_PRIORITY || priority<MIN_PRIORITY)
        priority = (MAX_PRIORITY + MIN_PRIORITY)/2;
    pcb[pid].priority = priority;
}

int isForeground(int pid){
    return !pcb[pid].ground;
}

void printAllProcess(){
    gPrint("These are your all time processes:");
    gNewline();
    for (int i = 0; i < processCount; i++)
    {
        gPrint("PID: ");
        gPrintDec(pcb[i].pid);
        gNewline();
    }

}

