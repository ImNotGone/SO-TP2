#include <libs/processManager.h>
#include <libs/scheduler.h>
#include <ADTS/queueADT.h>
#include <ADTS/priorityQueueADT.h>
#include <drivers/RTC.h>


#define KERNEL_PID -1

static queueADT readyQueue = NULL;
static queueADT blockedQueue = NULL;
static int started = 0;
static pid_t activePid = KERNEL_PID;
static PCBType * activeProcess = NULL;
static int gusts=0;

// Priority queue for sleeping processes, ordered by wakeup time
static pQueueADT sleepingQueue = NULL;

static void idle();

static int64_t compareTime(void * a, void * b) {
    return *((int64_t *) a) - *((int64_t *) b);
}

queueADT getQueue(){
    return readyQueue;
}

void printProcess(PCBType * pcb);

void freeProcess(PCBType * process);

void startScheduler(){
    started=1;
    readyQueue = newQueue(sizeof(PCBType *), comparePCB);
    blockedQueue = newQueue(sizeof(PCBType *), comparePCB);
    sleepingQueue = newPQueue(compareTime, sizeof(PCBType *), sizeof(uint64_t));

    // Create idle process
    newProcess((uint64_t) idle, 0, 1, 0, NULL);
}

// Adds a process to the sleeping queue
void addToSleepingQueue(PCBType * process, uint64_t seconds) {

    if (process == NULL || process->status != READY) {
        return;
    }

    uint64_t wakeupTime = getTotalSeconds() + seconds;

    pushPq(sleepingQueue, &process, &wakeupTime);
}

// Wakes up all processes that have a wakeup time before the current time
void wakeUpProcesses() {

    if (sleepingQueue == NULL || isEmptyPq(sleepingQueue)) {
        return;
    }

    uint64_t currentTime = getTotalSeconds();
    
    uint64_t toWakeUpSize = 0;
    PCBType ** toWakeUp = getElementsLessThan(sleepingQueue, &currentTime, &toWakeUpSize);

    for (int i = 0; i < toWakeUpSize; i++) {
        toWakeUp[i]->status = READY;
    }
}

// Iterates over the ready queue to see if there are any processes that are ready to run
int hasReadyProcess() {
    if (readyQueue == NULL || isEmpty(readyQueue)) {
        return 0;
    }

    PCBType * process = NULL;
    int hasReady = 0;

    toBegin(readyQueue);

    while (hasNext(readyQueue)) {
        next(readyQueue, &process);

        if (process->status == READY && process->rip != (uint64_t)idle) {
            return 1;
        }
    }

    return hasReady;
}

uint64_t switchContext(uint64_t rsp){
    //in this case there should be no saving of the Kernels context
    if (!started) {
        return rsp;
    }

    
    if(activePid == KERNEL_PID){
        activePid = 0;
        //shell
        // a lo mejor conviene hacer peek aca
        dequeue(readyQueue, &activeProcess);

        // If its idle, ignore it
        if (activeProcess->rip == (uint64_t)idle) {
            queue(readyQueue, &activeProcess);
            dequeue(readyQueue, &activeProcess);
        }


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

            // If its idle and there are other processes, ignore it
            if (activeProcess->rip == (uint64_t)idle && hasReadyProcess()) {
                queue(readyQueue, &activeProcess);
                continue;
            }

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

    if (pcb == NULL || pcb->pid == 0) {
        return;
    }

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
    gPrint("Waiting on this process: ");
    gNewline();

    // Iterate over waiting processes
    toBegin(pcb->waiting_processes);
    pid_t pid;
    while (hasNext(pcb->waiting_processes)) {
        next(pcb->waiting_processes, &pid);

        gPrint("PID: ");
        gPrintDec(pid);
        gNewline();
    }

    gPrint("------");
    gNewline();
}

void freeProcess(PCBType * process){
    for(int i = 0 ; i < process->argc ; i++){
        free(process->argv[i]);
    }

    free(process->argv);
    free((void *) process->stack_base - STACK_SIZE);
    freeQueue(process->waiting_processes);
    free(process);
}

PCBType * getActiveProcess(){
    return activeProcess;
}

static void idle() {
    while (1){
        _hlt();
    }
}
