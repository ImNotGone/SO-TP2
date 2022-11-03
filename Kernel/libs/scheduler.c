#include <libs/processManager.h>
#include <libs/scheduler.h>
#include <ADTS/queueADT.h>

#define KERNEL_PID -1

static queueADT readyQueue = NULL;
static queueADT blockedQueue = NULL;
static int started = 0;
static pid_t activePid = KERNEL_PID;
static PCBType * activeProcess = NULL;
static int gusts=0;

static void idle();

queueADT getQueue(){
    return readyQueue;
}

void printProcess(PCBType * pcb);

void freeProcess(PCBType * process);

void startScheduler(){
    started=1;
    readyQueue = newQueue(sizeof(PCBType *), comparePCB);
    blockedQueue = newQueue(sizeof(PCBType *), comparePCB);
}

uint64_t switchContext(uint64_t rsp){
    //in this case there should be no saving of the Kernels context
    if (!started){
        return rsp;
    }

    if(activePid == KERNEL_PID){
        activePid = 0;
        //shell
        // a lo mejor conviene hacer peek aca
        dequeue(readyQueue, &activeProcess);
        gusts = activeProcess->priority;
        activePid = activeProcess->pid;
        return activeProcess->rsp;
    }

    //saves previous context
    //remove killed processes from queue
    activeProcess->rsp = rsp;
    if(!gusts){
        if (activeProcess->status != KILLED){
            queue(readyQueue, &activeProcess);
        }else{
            freeProcess(activeProcess);
        }
    }

    while(!gusts){
        if(!dequeue(readyQueue, &activeProcess)){
            //maybe do something?
        }

        if(activeProcess->status == READY){
            activePid = activeProcess->pid;
            gusts = activeProcess->priority;
            //return activeProcess->rsp;
        }else if (activeProcess->status == BLOCKED){
            queue(readyQueue, &activeProcess);
        }else if(activeProcess->status == KILLED){
            freeProcess(activeProcess);
        }

    }

    gusts--;
    return activeProcess->rsp;
}

void addToReadyQueue(PCBType ** process){
    queue(readyQueue, process);
}

pid_t getActivePid(){
    return activePid;
}

void yield(){
    gusts = 0;
    _int20();
}

PCBType * find(pid_t pid){
    if (activePid == pid){
        return activeProcess;
    }

    toBegin(readyQueue);
    PCBType * aux;
    while(hasNext(readyQueue)){
        next(readyQueue, &aux);
        if(aux->pid == pid)
            return aux;
    }
    return NULL;
}

void printPs(){
    gNewline();
    gPrint("---------");
    gNewline();
    toBegin(readyQueue);
    PCBType * pcb;
    while(hasNext(readyQueue)){
        next(readyQueue, &pcb);
        printProcess(pcb);
    }
    printProcess(activeProcess);
}

void printProcess(PCBType * pcb){
    gPrint("NAME: ");
    gPrint(pcb->name);
    gNewline();
    gPrint("PID: ");
    gPrintDec(pcb->pid);
    gNewline();
    gPrint("Priority: ");
    gPrintDec(pcb->priority);
    gNewline();
    gPrint("Stack base: ");
    gPrintDec(pcb->stack_base);
    gNewline();
    gPrint("Stack pointer: ");
    gPrintDec(pcb->rsp);
    gNewline();
    gPrint("Ground: ");
    gPrint(!pcb->ground ? "foreground" : "background");
    gNewline();
    gPrint("Status: ");
    gPrintDec(pcb->status);
    gNewline();
    gPrint("------");
    gNewline();
}

void freeProcess(PCBType * process){
    for(int i = 0 ; i < process->argc ; i++){
        free(process->argv[i]);
    }
    free(process->argv);
    free((void*)(process->stack_base - STACK_SIZE));
    free(process);
}

static void idle() {
    while (1){
        _hlt();
    }
}
