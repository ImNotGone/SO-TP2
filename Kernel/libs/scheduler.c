#include <libs/processManager.h>
#include <libs/scheduler.h>
#include <ADTS/queueADT.h>
#include <ADTS/priorityQueueADT.h>
#include <drivers/RTC.h>
#include <drivers/graphics.h>


#define KERNEL_PID -1

static queueADT processQueue = NULL;
static int started = 0;
static pid_t activePid = KERNEL_PID;
static PCBType * activeProcess = NULL;
static int gusts=0;

static PCBType * blockedOnInput = NULL;

// Priority queue for sleeping processes, ordered by wakeup time
static pQueueADT sleepingQueue = NULL;

static void idle();

static int64_t compareTime(void * a, void * b) {
    return *((int64_t *) a) - *((int64_t *) b);
}

queueADT getQueue(){
    return processQueue;
}

void printProcess(PCBType * pcb);

void freeProcess(PCBType * process);

void startScheduler(){
    started=1;
    processQueue = newQueue(sizeof(PCBType *), comparePCB);
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
        if (toWakeUp[i]->status != KILLED) {
            toWakeUp[i]->status = READY;
        }
    }

    free(toWakeUp);
}

// Iterates over the ready queue to see if there are any processes that are ready to run
int hasReadyProcess() {
    if (processQueue == NULL || isEmpty(processQueue)) {
        return 0;
    }

    PCBType * process = NULL;
    int hasReady = 0;

    toBegin(processQueue);

    while (hasNext(processQueue)) {
        next(processQueue, &process);

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
        dequeue(processQueue, &activeProcess);

        // If its idle, ignore it
        if (activeProcess->rip == (uint64_t)idle) {
            queue(processQueue, &activeProcess);
            dequeue(processQueue, &activeProcess);
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
            queue(processQueue, &activeProcess);
        }else{
            freeProcess(activeProcess);
        }
    }

    while(!gusts){
        if(!dequeue(processQueue, &activeProcess)){
            //maybe do something?
        }

        if(activeProcess->status == READY){

            // If its idle and there are other processes, ignore it
            if (activeProcess->rip == (uint64_t)idle && hasReadyProcess()) {
                queue(processQueue, &activeProcess);
                continue;
            }

            activePid = activeProcess->pid;
            gusts = activeProcess->priority;
            //return activeProcess->rsp;
        }else if (activeProcess->status == BLOCKED){
            queue(processQueue, &activeProcess);
        }else if(activeProcess->status == KILLED){
            freeProcess(activeProcess);
        }

    }

    // // If the active process has not consumed its quantum, continue with it
    // if (activeProcess != NULL && gusts > 0) {
    //     gusts--;
    //     return rsp;
    // }
    //
    // // If the active process has consumed its quantum, save its context and get the next process
    // if (activeProcess != NULL && gusts == 0) {
    //     activeProcess->rsp = rsp;
    //     queue(readyQueue, &activeProcess);
    // }
    //
    // // Get the next process
    // // If there are no process left in the queue
    // // run the active process again
    // // If active process is NULL, run idle
    //
    // // Get the next process
    // if (!isEmpty(readyQueue)) {
    //     dequeue(readyQueue, &activeProcess);
    //
    //     activePid = activeProcess->pid;
    //     gusts = activeProcess->priority;
    //     return activeProcess->rsp;
    //
    // }
    //
    // // Run active process again
    // if (activeProcess != NULL) {
    //     gusts = activeProcess->priority - 1;
    //     return rsp;
    // }
    //
    // // Run idle
    // idle();

    gusts--;
    return activeProcess->rsp;
}

void addToReadyQueue(PCBType ** process){
    queue(processQueue, process);
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

    toBegin(processQueue);
    PCBType * aux;
    while(hasNext(processQueue)){
        next(processQueue, &aux);
        if(aux->pid == pid)
            return aux;
    }
    return NULL;
}

void printPs(){
    gPrint(" == STATUS ==");
    gNewline();
    gPrint("  0 = READY");
    gNewline();
    gPrint("  1 = BLOCKED");
    gNewline();
    gPrint("  2 = KILLED");
    gNewline();
    gPrint(" ============");
    PCBType * pcb;
    toBegin(processQueue);
    while(hasNext(processQueue)){
        next(processQueue, &pcb);
        printProcess(pcb);
    }
    printProcess(activeProcess);
    gNewline();
}

void printProcess(PCBType * pcb){

    if (pcb == NULL || pcb->pid == 0) {
        return;
    }

    gNewline();
    gPrint("NAME: ");
    gPrint(pcb->name);
    gNewline();
    gPrint("PID: ");
    gPrintDec(pcb->pid);
    gNewline();
    gPrint("Priority: ");
    gPrintDec(pcb->priority);
    gPrint(" | ");
    gPrint("Stack base: 0x");
    gPrintHex(pcb->stack_base);
    gPrint(" | ");
    gPrint("Stack pointer: 0x");
    gPrintHex(pcb->rsp);
    gPrint(" | ");
    gPrint("Ground: ");
    gPrint(!pcb->ground ? "foreground" : "background");
    gPrint(" | ");
    gPrint("Status: ");
    gPrintDec(pcb->status);

    if(!isEmpty(pcb->waiting_processes)) {
        gNewline();
        gPrint("WAITING ON THIS PROCESS ARE (pids): ");
        // Iterate over waiting processes
        toBegin(pcb->waiting_processes);
        pid_t pid;
        while (hasNext(pcb->waiting_processes)) {
            next(pcb->waiting_processes, &pid);
            gPrintDec(pid);
            gPrint(" | ");
        }
    }
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

void setBlockedOnInput(PCBType * process){
    blockedOnInput = process;
}

void wakeUpBlockedOnInput() {
    if(blockedOnInput != NULL){
        blockedOnInput->status = READY;
        blockedOnInput = NULL;
    }
}

static void idle() {
    while (1){
        _hlt();
    }
}
