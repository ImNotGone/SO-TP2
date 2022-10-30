#include <libs/scheduler.h>
#define KERNEL_PID -1

static queueADT readyQueue = NULL;
static queueADT blockedQueue = NULL;
static int processCount =0;
static int started = 0;
static int activePid = KERNEL_PID;
static Pdata shell;
static Pdata * activeProcess = NULL;

void startScheduler(){
    started=1;
    readyQueue = newQueue();
    toBegin(readyQueue);
    blockedQueue = newQueue();

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
        toReturn->pcb->rsp;
        queue(readyQueue, *toReturn);
        shell = *toReturn;
        return toReturn->pcb->rsp;
    }


    //saves previous context
    //ver de no agregarlo siempre si esta killed por ejemplo
    activeProcess->pcb->rsp = rsp;
    if (strcmp(toReturn->pcb->status, "killed"))
        queue(readyQueue, *activeProcess);

    int proceed = 0;
    while(!proceed){
        if(dequeue(readyQueue, toReturn)==NULL){
            //maybe do something?
        }

        if(!strcmp(toReturn->pcb->status, "ready")){
            proceed=1;
        }else if (!strcmp(toReturn->pcb->status, "blocked"))
        {
            queue(readyQueue, *toReturn);
        }

    }

    //set next process
    activeProcess = toReturn;
    activePid = toReturn->pid;
    return activeProcess->pcb->rsp;
}

void addToReadyQueue(Pdata process){
    queue(readyQueue, process);
}

int getActivePid(){
    return activePid;
}

void removeFromReadyQueue(){
    //poner algun idle, o algo
    //hacer yield
}

void yield(){
    _int20();
}


