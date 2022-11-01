#include <libs/scheduler.h>
#define KERNEL_PID -1

static queueADT readyQueue = NULL;
static queueADT blockedQueue = NULL;
static int processCount =0;
static int idlePid;
static int started = 0;
static int activePid = KERNEL_PID;
static Pdata shell;
static Pdata * activeProcess = NULL;
static int gusts=0;

static void idle();

void startScheduler(){
    started=1;
    readyQueue = newQueue();

    //char *argv[] = {NULL};
    //idlePid = newProcess(idle, 1, 0, 0, argv);
    //exec(idlePid);

    toBegin(readyQueue);
}

static void idle() {
    while (1){
        _hlt();    
    }
}

uint64_t switchContext(uint64_t rsp){
    //in this case there should be no saving of the Kernels context
    if (!started){
        return rsp;
    }

    

    Pdata aux;
    Pdata * toReturn = &aux;

    if(activePid == KERNEL_PID){
        activePid = 0;
        //shell
        // a lo mejor conviene hacer peek aca
        dequeue(readyQueue, toReturn);
        queue(readyQueue, *toReturn);
        shell = *toReturn;
        gusts = shell.pcb->priority;
        activePid = toReturn->pid;
        activeProcess = toReturn;
        return toReturn->pcb->rsp;
    }

    //saves previous context
    //remove killed processes from queue
    activeProcess->pcb->rsp = rsp;
    if(!gusts){
        if (activeProcess->pcb->status != KILLED){
            queue(readyQueue, *activeProcess);
        }

    }

    while(!gusts){
        if(dequeue(readyQueue, toReturn)==NULL){
            //maybe do something?
        }

        if(toReturn->pcb->status == READY){
            activeProcess = toReturn;
            activePid = toReturn->pid;
            gusts = activeProcess->pcb->priority;
        }else if (toReturn->pcb->status == BLOCKED){
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


