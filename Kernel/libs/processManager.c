// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <types.h>
#include <libs/processManager.h>
#include <libs/scheduler.h>
#include <lib.h>

#define MAX_PROCESS 30
#define KERNEL_PID -1
#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

int64_t comparePid(void* pid1, void* pid2);

static pid_t nextPid = 0;

static PCBType * pcb;

pid_t newProcess(uint64_t rip, int ground, int priority, int argc, char * argv[]){

    pcb = malloc(sizeof(PCBType));

    pcb->stack_base = (uint64_t) malloc(STACK_SIZE) + STACK_SIZE;
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


    if(getActivePid()== KERNEL_PID)
        pcb->status = READY;
    else{
        pcb->status = BLOCKED;
    }

    pcb->priority = priority;
    pcb->pid = nextPid;

    pcb->waiting_processes = newQueue(sizeof(pid_t), comparePid);

    pcb->ppid = getActivePid();
    pcb->rsp = createProcess(pcb->stack_base, pcb->rip, pcb->argc, pcb->argv, false);

    pcb->fd[STDIN] = STDIN;
    pcb->fd[STDOUT] = STDOUT;
    pcb->fd[STDERR] = STDERR;


    addToReadyQueue(&pcb);

    return nextPid++;
}

int64_t comparePCB(void * pcb1, void * pcb2) {
    if(pcb1 == NULL || pcb2 == NULL) {
        return (pcb1 != NULL) - (pcb2 != NULL);
    }
    PCBType a = *(PCBType *)pcb1;
    PCBType b = *(PCBType *)pcb2;
    return (a.pid > b.pid) - (a.pid < b.pid);
}

int64_t comparePid(void * pid1, void * pid2) {
    if(pid1 == NULL || pid2 == NULL) {
        return (pid1 != NULL) - (pid2 != NULL);
    }
    pid_t a = *(pid_t *)pid1;
    pid_t b = *(pid_t *)pid2;
    return (a > b) - (a < b);
}

void exec(pid_t pid){

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
    //addToReadyQueue(&pcb);

}

int64_t killProcess(pid_t pid) {
    PCBType * process = find(pid);

    // Validate if the process exists
    if(process == NULL) {
        return -1;
    }

    process->status = KILLED;

    // Unblock all the processes that were waiting for this process
    toBegin(process->waiting_processes);
    while(hasNext(process->waiting_processes)) {
        pid_t pid;
        next(process->waiting_processes, &pid);

        unblockProcess(pid);
    }

    if (process->pid == getActivePid()) {
        yield();
    }

    return 0;
}

int64_t sleepProcess(pid_t pid, uint64_t seconds) {
    PCBType * process = find(pid);

    // Validate if the process exists
    if(process == NULL || process->status != READY) {
        return -1;
    }

    addToSleepingQueue(process, seconds);
    process->status = BLOCKED;

    if (process->pid == getActivePid()) {
        yield();
    }

    return 0;
}

int64_t blockProcess(pid_t pid) {
    PCBType * process = find(pid);

    // Validate if the process exists
    if(process == NULL || process->status != READY) {
        return -1;
    }

    process->status = BLOCKED;
    if (pid == getActivePid()) {
        yield();
    }

    return 0;
}

int64_t blockProcessOnInput(pid_t pid) {
    PCBType * process = find(pid);

    setBlockedOnInput(process);

    return blockProcess(pid);
}

int64_t unblockProcess(pid_t pid) {
    PCBType * process = find(pid);

    // Validate if the process exists
    if (process == NULL || process->status == KILLED) {
        return -1;
    }

    process->status = READY;

    return 0;
}

int64_t waitProcess(pid_t pid) {
    PCBType * process = find(pid);
    pid_t activePid = getActivePid();

    // Validate if the process exists
    if (process == NULL || process->status == KILLED) {
        return -1;
    }

    // Add the current process to the waiting queue of the process
    queue(process->waiting_processes, &activePid);

    // if(process->status != KILLED) {
         blockProcess(activePid);
    // }

    return 0;
}

void changePriority(pid_t pid, int priority) {
    if(priority>MAX_PRIORITY || priority<MIN_PRIORITY)
        priority = (MAX_PRIORITY + MIN_PRIORITY)/2;

    PCBType * process = find(pid);
    if(process == NULL) return;
    process->priority = priority;
}

void printAllProcess(){
    printPs();
}

int dup(pid_t pid, fd_t prev, fd_t new){
    // if(prev != STDOUT && new != STDIN){
    //     return -1;
    // }
    PCBType * process = find(pid);
    process->fd[prev] = new;
    return 1;
}

int64_t fork(uint64_t rip, uint64_t rsp) {
    PCBType * parent = getActiveProcess();
    pcb = malloc(sizeof(PCBType));

    pcb->stack_base = (uint64_t) malloc(STACK_SIZE) + STACK_SIZE;
    // COPY parent stack
    memcpy((void *)(pcb->stack_base - STACK_SIZE), (void *)(parent->stack_base - STACK_SIZE), STACK_SIZE);
    pcb->rip = rip;
    pcb->argc = parent->argc;
    pcb->ground = parent->ground;

    pcb->argv = malloc((pcb->argc + 1) * sizeof(char *));
    for (int i = 0; i < pcb->argc; i++){
        pcb->argv[i] = malloc(strlen(parent->argv[i])+1);
        strcpy(pcb->argv[i], parent->argv[i]);
    }
    pcb->argv[pcb->argc] = (void *)0;
    pcb->name = pcb->argv[0];


    if(getActivePid()== KERNEL_PID)
        pcb->status = READY;
    else{
        pcb->status = BLOCKED;
    }

    pcb->priority = parent->priority;
    pcb->pid = nextPid;

    pcb->waiting_processes = newQueue(sizeof(pid_t), comparePid);

    pcb->ppid = parent->pid;
    pcb->rsp = createProcess(pcb->stack_base - (parent->stack_base - rsp), rip, pcb->argc, pcb->argv, true);
    // TODO: CHANGE IF VARIABLE FDS
    pcb->fd[STDIN] = parent->fd[STDIN];
    pcb->fd[STDOUT] = parent->fd[STDOUT];
    pcb->fd[STDERR] = parent->fd[STDERR];

    addToReadyQueue(&pcb);

    return nextPid++;

}
