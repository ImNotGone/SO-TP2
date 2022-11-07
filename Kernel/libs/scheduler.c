// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ADTS/priorityQueueADT.h>
#include <ADTS/queueADT.h>
#include <drivers/RTC.h>
#include <drivers/graphics.h>
#include <libs/processManager.h>
#include <libs/scheduler.h>

#define KERNEL_PID -1

static queueADT processQueue = NULL;
static int started = 0;
static pid_t activePid = KERNEL_PID;
static PCBType *activeProcess = NULL;
static int gusts = 0;

static PCBType *blockedOnInput = NULL;

// Priority queue for sleeping processes, ordered by wakeup time
static pQueueADT sleepingQueue = NULL;

static void idle();

static int64_t compareTime(void *a, void *b) {
    return *((int64_t *)a) - *((int64_t *)b);
}

queueADT getQueue() { return processQueue; }

void printProcess(PCBType *pcb);

void freeProcess(PCBType *process);

void startScheduler() {
    started = 1;
    processQueue = newQueue(sizeof(PCBType *), comparePCB);
    sleepingQueue = newPQueue(compareTime, sizeof(PCBType *), sizeof(uint64_t));

    // Create idle process
    newProcess((uint64_t)idle, 1, 1, 0, NULL);
}

// Adds a process to the sleeping queue
void addToSleepingQueue(PCBType *process, uint64_t seconds) {

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
    PCBType **toWakeUp =
        getElementsLessThan(sleepingQueue, &currentTime, &toWakeUpSize);

    for (int i = 0; i < toWakeUpSize; i++) {
        if (toWakeUp[i]->status == SLEEPING) {
            toWakeUp[i]->status = READY;
        }
    }

    free(toWakeUp);
}

// Removes a process from the sleeping queue if it is there
void removeFromSleepingQueue(PCBType *process) {
    
    if (process == NULL) {
        return;
    }

    removePq(sleepingQueue, &process);
}

    
// Iterates over the ready queue to see if there are any processes that are
// ready to run
int hasReadyProcess() {
    if (processQueue == NULL || isEmpty(processQueue)) {
        return 0;
    }

    PCBType *process = NULL;
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

uint64_t switchContext(uint64_t rsp) {
    // in this case there should be no saving of the Kernels context
    if (!started) {
        return rsp;
    }

    if (activePid == KERNEL_PID) {
        activePid = 0;
        // shell
        //  a lo mejor conviene hacer peek aca
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

    // saves previous context
    // remove killed processes from queue
    activeProcess->rsp = rsp;
    if (!gusts) {
        if (activeProcess->status != KILLED) {
            queue(processQueue, &activeProcess);
        } else {
            freeProcess(activeProcess);
        }
    }

    while (!gusts) {
        if (!dequeue(processQueue, &activeProcess)) {
            // maybe do something?
        }

        if (activeProcess->status == READY) {

            // If its idle and there are other processes, ignore it
            if (activeProcess->rip == (uint64_t)idle && hasReadyProcess()) {
                queue(processQueue, &activeProcess);
                continue;
            }

            activePid = activeProcess->pid;
            gusts = activeProcess->priority;
            // return activeProcess->rsp;
        } else if (activeProcess->status == BLOCKED || activeProcess->status == SLEEPING) {
            queue(processQueue, &activeProcess);
        } else if (activeProcess->status == KILLED) {
            freeProcess(activeProcess);
        }
    }

    gusts--;
    return activeProcess->rsp;
}

void addToReadyQueue(PCBType **process) { queue(processQueue, process); }

pid_t getActivePid() { return activePid; }

void yield() {
    gusts = 0;
    _int20();
}

PCBType *findProcess(pid_t pid) {
    if (activePid == pid) {
        return activeProcess;
    }

    toBegin(processQueue);
    PCBType *aux;
    while (hasNext(processQueue)) {
        next(processQueue, &aux);
        if (aux->pid == pid)
            return aux;
    }
    return NULL;
}

PCBType * removeProcess(pid_t pid) {
    if (activePid == pid) {
        return activeProcess;
    }

    PCBType * aux = malloc(sizeof(PCBType));
    if(aux == NULL) {
        return NULL;
    }
    aux->pid = pid;
    if(!remove(processQueue, &aux)) {
        return NULL;
    }

    return aux;
}

int64_t copyPCB(TProcInfo *to, PCBType *pcb) {
    to->pid = pcb->pid;
    to->status = pcb->status;
    to->priority = pcb->priority;
    to->rsp = pcb->rsp;
    to->stackBase = pcb->stack_base;
    to->ground = pcb->ground;

    if (pcb->name != NULL) {
        to->name = malloc(strlen(pcb->name) + 1);

        if (to->name == NULL) {
            return -1;
        }

        strcpy(to->name, pcb->name);
    } else {
        to->name = NULL;
    }

    // Get the size of the queue
    uint64_t waitingQueueSize = 0;
    pid_t waitingProcess;
    toBegin(pcb->waiting_processes);
    while (hasNext(pcb->waiting_processes)) {
        next(pcb->waiting_processes, &waitingProcess);
        waitingQueueSize++;
    }

    // Waiting queue
    if (waitingQueueSize > 0) {

        to->waitingProcessesSize = waitingQueueSize;

        to->waitingProcesses = malloc(sizeof(pid_t) * waitingQueueSize);

        if (to->waitingProcesses == NULL) {
            return -1;
        }

        pid_t waitingProcess;
        toBegin(pcb->waiting_processes);
        int j = 0;
        while (hasNext(pcb->waiting_processes)) {
            next(pcb->waiting_processes, &waitingProcess);
            to->waitingProcesses[j] = waitingProcess;
            j++;
        }

    } else {
        to->waitingProcessesSize = 0;
        to->waitingProcesses = NULL;
    }

    return 0;
}

TProcInfo *procDump(uint64_t *size) {

    if (processQueue == NULL) {
        return NULL;
    }

    // Get the size of the queue
    uint64_t queueSize = 0;
    PCBType *pcb;
    toBegin(processQueue);
    while (hasNext(processQueue)) {
        next(processQueue, &pcb);
        queueSize++;
    }

    TProcInfo *processes = malloc(sizeof(TProcInfo) * (queueSize));


    if (processes == NULL) {
        *size = 0;
        return NULL;
    }

    // Add the active process
    if (copyPCB(&processes[0], activeProcess) == -1) {
        *size = 0;
        return NULL;
    }


    pcb = NULL;
    toBegin(processQueue);
    int i = 0;
    while (hasNext(processQueue)) {
        next(processQueue, &pcb);

        if(pcb->pid != 0) {
            if (copyPCB(&processes[i + 1], pcb) == -1) {

                for (int j = 0; j < i; j++) {
                    free(processes[j].name);
                    free(processes[j].waitingProcesses);
                }

                *size = 0;
                return NULL;
            }
            i++;
        }
    }

    *size = queueSize;
    return processes;
}

PCBType *getActiveProcess() { return activeProcess; }

void setBlockedOnInput(PCBType *process) { blockedOnInput = process; }

void wakeUpBlockedOnInput() {
    if (blockedOnInput != NULL) {
        blockedOnInput->status = READY;
        blockedOnInput = NULL;
    }
}

void killForegroundProcess() {
    if (activeProcess->ground == 0 && activeProcess->pid != 1) {
        gPrint("^C [");
        gPrintDec(activeProcess->pid);
        gPrint("]");
        gPrint(" Killed");
        gNewline();

        killProcess(activeProcess->pid);
    }

    toBegin(processQueue);
    PCBType *aux;
    while (hasNext(processQueue)) {
        next(processQueue, &aux);

        if(aux->ground == 0 && aux->pid != 1){
            gPrint("^C [");
            gPrintDec(aux->pid);
            gPrint("]");
            gPrint(" Killed");
            gNewline();

            killProcess(aux->pid);
        }
    }
}

static void idle() {
    while (1) {
        _hlt();
    }
}
