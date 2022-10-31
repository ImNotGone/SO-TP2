#include <libs/scheduler.h>
#define KERNEL_PID -1

static queueADT readyQueue = NULL;
static queueADT blockedQueue = NULL;
static int processCount =0;
static int started = 0;
static int activePid = KERNEL_PID;
static Pdata shell;
static Pdata * activeProcess = NULL;
static int gusts=0;

void startScheduler(){
    started=1;
    readyQueue = newQueue();
    toBegin(readyQueue);
    //blockedQueue = newQueue();

}

uint64_t switchContext(uint64_t rsp){
    //in this case there should be no saving of the Kernels context
    if (!started){
        return rsp;
    }

    Pdata * toReturn = NULL;

    if(activePid == KERNEL_PID){
        activePid = 0;
        //shell
        // a lo mejor conviene hacer peek aca
        dequeue(readyQueue, toReturn);
        queue(readyQueue, *toReturn);
        shell = *toReturn;
        gusts = shell.pcb->priority;
        return toReturn->pcb->rsp;
    }

    //saves previous context
    //remove killed processes from queue
    activeProcess->pcb->rsp = rsp;
    if(!gusts){
        if (strcmp(activeProcess->pcb->status, "killed")){
            queue(readyQueue, *activeProcess);
        }

    }

    while(!gusts){
        if(dequeue(readyQueue, toReturn)==NULL){
            //maybe do something?
        }

        if(!strcmp(toReturn->pcb->status, "ready")){
            activeProcess = toReturn;
            activePid = toReturn->pid;
            gusts = activeProcess->pcb->priority;
        }else if (!strcmp(toReturn->pcb->status, "blocked")){
            queue(readyQueue, *toReturn);
        }

    }

    gusts--;
    return activeProcess->pcb->rsp;
}

void addToReadyQueue(Pdata process){
    queue(readyQueue, process);
}

int getActivePid(){
    return activePid;
}

void yield(){
    _int20();
}


