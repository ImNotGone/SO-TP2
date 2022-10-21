#include <libs/processManager.h>

#define MAX_PROCESS 5
#define STACK_SIZE 4048
#define KERNEL_PID -1

typedef struct processData{
    uint16_t pid, ppid;
    uint64_t rsp, stack_base, rip;
    int argc;
    char ** argv;
} PCBType;

static PCBType pcb[MAX_PROCESS]={0};
static int stack[MAX_PROCESS][STACK_SIZE]={0};
static int processCount = 0;
static int pid = 0;
static int activePid = KERNEL_PID;

uint64_t newProcess( uint64_t rip, int argc, char * argv[]){
    pcb[processCount].stack_base = stack[processCount]+ STACK_SIZE;
    pcb[processCount].rip = rip;
    pcb[processCount].argc = argc;
    pcb[processCount].argv = argv;
    pcb[processCount].pid = pid;

    pcb[processCount].rsp =createProcess(pcb[processCount].stack_base, rip, argc, argv);

    processCount++;
    return pid++;
}

//en el context switch debo chequear primero si el pid es 0 => entonces no debo switchear
uint64_t schedule(uint64_t rsp){
    //in this case there should be no saving of the Kernels context
    if (processCount == 0){
        return rsp;
    }

    if(activePid == KERNEL_PID){
        activePid = 0;
        return pcb[activePid].rsp;
    }

    return rsp;

    //else, fully context switch
}
