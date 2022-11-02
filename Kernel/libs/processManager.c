// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libs/processManager.h>
#include <libs/scheduler.h>

#define MAX_PROCESS 30
#define KERNEL_PID -1
#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

static int processCount = 0;

uint64_t strlen(char *string);
void strcpy(char *string, char *target);

static PCBType * pcb;


uint64_t newProcess(uint64_t rip, int ground, int priority, int argc, char * argv[]){

    pcb = malloc(sizeof(PCBType));

    pcb->stack_base = malloc(STACK_SIZE) + STACK_SIZE;
    pcb->rip = rip;
    pcb->argc = argc;
    pcb->ground = ground;

    pcb->argv = malloc((argc + 1) * sizeof(char *));
    for (int i = 0; i < argc; i++){
        pcb->argv[i] = malloc(strlen(argv[i])+1);
        strcpy(pcb->argv[i], argv[i]);
    }
    pcb->argv[argc] = (void *)0;
    pcb->name = pcb->argv[0];

    pcb->status= READY;
    pcb->priority=priority;
    pcb->pid = processCount;
    pcb->ppid = getActivePid();
    pcb->rsp = createProcess(pcb->stack_base, pcb->rip, argc, argv);

    addToReadyQueue(&pcb);
    if(pcb->ground == 0  && pcb->pid != 0){
        block(pcb->ppid);
    }

    return processCount++;
}

void exec(uint64_t pid){

    //Pdata process = {&pcb[pid], pcb[pid].pid};
    // PCBType * process = NULL;
    // process = listfind(pcbs, pid);

    // //que hacemos si no lo encuentra?

    // //toSend = {process, process->pid};

    // //addToReadyQueue(process);
    // addToReadyQueue(process);

    // //si es foreground:
    // if(process->ground == 0){
    //     block(process->ppid);
    // }
}

void killProcess(uint64_t pid){
    PCBType * process = find(pid);
    process->status = KILLED;

    // si es foreground:
    if(process->ground == 0)
        unblock(process->ppid);
}

void block(uint64_t pid){
    PCBType * process = find(pid);
    process->status = BLOCKED;
    if (pid == getActivePid()){
        yield();
    }
}

void unblock(uint64_t pid){
    PCBType * process = find(pid);
    process->status = READY;
}

void changePriority(int pid, int priority){
    if(priority>MAX_PRIORITY || priority<MIN_PRIORITY)
        priority = (MAX_PRIORITY + MIN_PRIORITY)/2;

    PCBType * process = find(pid);
    process->priority = priority;
}

int isForeground(int pid){
    PCBType * process = find(pid);
    return !process->ground;
}

void printAllProcess(){
    printPs();
}

// -- aux string functions --

uint64_t strlen(char *string){
    int i = 0;
    while(string[i++]);
    return i - 1;
}

void strcpy(char *string, char *target){
    while(*target != 0){
        *(string++) = *(target++);
    }
    *string = 0;
}
