// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libs/processManager.h>
#include <libs/scheduler.h> //dont move from here, necessary to avoid .h recursion
#include <ADTS/listADT.h>

#define MAX_PROCESS 30
#define STACK_SIZE 4096
#define KERNEL_PID -1
#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

//static PCBType pcb[MAX_PROCESS]={{0}};
static listADT pcbs;
//static int stack[MAX_PROCESS][STACK_SIZE]={{0}};
static int processCount = 0;

uint64_t strlen(char *string);
void strcpy(char *string, char *target);
int find(PCBType ** process, int pid);

void initProcessManager(){
    pcbs = listnewList();
}

uint64_t newProcess(uint64_t rip, int ground, int priority, int argc, char * argv[]){

    //stack size -+1?
    // pcb[processCount].stack_base = stack[processCount]+STACK_SIZE;
    // pcb[processCount].rip = rip;
    // pcb[processCount].argc = argc;
    // pcb[processCount].ground = ground;
    // pcb[processCount].argv = argv;
    // pcb[processCount].status= READY;
    // pcb[processCount].name = argv[0];
    // pcb[processCount].priority=priority;
    // pcb[processCount].pid = processCount;
    // pcb[processCount].ppid = getActivePid();

    // pcb[processCount].rsp = createProcess(pcb[processCount].stack_base, pcb[processCount].rip, argc, argv);
    PCBType * pcb = malloc(sizeof(PCBType));
    pcb->stack_base = malloc(STACK_SIZE) + STACK_SIZE;
    pcb->rip = rip;
    pcb->argc = argc;
    pcb->ground = ground;

    pcb->argv = malloc(sizeof(argc + 1) * sizeof(char *));
    for (int i = 0; i < argc; i++){
        pcb->argv[i] = malloc(strlen(argv[i])+1);
        strcpy(pcb->argv[i], argv[i]);
    }
    pcb->argv[argc] = NULL;
    pcb->name = pcb->argv[0];

    pcb->status= READY;
    pcb->priority=priority;
    pcb->pid = processCount;
    pcb->ppid = getActivePid();
    pcb->rsp = createProcess(pcb->stack_base, pcb->rip, argc, argv);

    listadd(pcbs, pcb);

    return processCount++;
}

void exec(uint64_t pid){
    //Pdata process = {&pcb[pid], pcb[pid].pid};
    PCBType * process;
    find(&process, pid);
    //que hacemos si no lo encuentra?

    Pdata toSend = {process, process->pid};
    //addToReadyQueue(process);
    addToReadyQueue(toSend);

    //si es foreground:
    if(process->ground == 0){
        block(process->ppid);
    }
}

void killProcess(uint64_t pid){
    PCBType * process = malloc(sizeof(PCBType));
    find(&process, pid);
    //pcb[pid].status = KILLED;

    process->status = KILLED;

    // si es foreground:
    if(process->ground == 0)
        unblock(process->ppid);
}

void block(uint64_t pid){
    PCBType * process;
    find(&process, pid);

    process->status = BLOCKED;
    if (pid == getActivePid()){
        yield();
    }
}

void unblock(uint64_t pid){
    PCBType * process = malloc(sizeof(PCBType));
    find(&process, pid);
    process->status = READY;
}

void changePriority(int pid, int priority){
    if(priority>MAX_PRIORITY || priority<MIN_PRIORITY)
        priority = (MAX_PRIORITY + MIN_PRIORITY)/2;

    PCBType * process = malloc(sizeof(PCBType));
    find(&process, pid);

    process->priority = priority;
}

int isForeground(int pid){
    //return !pcb[pid].ground;
}

void printAllProcess(){
    // gPrint("These are your all time processes:");
    // gNewline();
    // gPrint("-----");
    // gNewline();
    // for (int i = 0; i < processCount; i++){
    //     gPrint("NAME: ");
    //     gPrint(pcb[i].name);
    //     gNewline();
    //     gPrint("PID: ");
    //     gPrintDec(pcb[i].pid);
    //     gNewline();
    //     gPrint("Priority: ");
    //     gPrintDec(pcb[i].priority);
    //     gNewline();
    //     gPrint("Stack base: ");
    //     gPrintDec(pcb[i].stack_base);
    //     gNewline();
    //     gPrint("Stack pointer: ");
    //     gPrintDec(pcb[i].rsp);
    //     gNewline();
    //     gPrint("Ground: ");
    //     gPrint(isForeground(pcb[i].ground)? "foreground": "background");
    //     gNewline();
    //     gPrint("Status: ");
    //     gPrintDec(pcb[i].status);
    //     gNewline();
    //     gPrint("------");
    //     gNewline();
    // }

    gNewline();
    gPrint("---------");
    gNewline();
    listtoBeginAsc(pcbs);
    PCBType * pcb;
    while(listhasNextAsc(pcbs)){
        pcb = listnextAsc(pcbs);
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
        gPrint(isForeground(pcb->ground)? "foreground": "background");
        gNewline();
        gPrint("Status: ");
        gPrintDec(pcb->status);
        gNewline();
        gPrint("------");
        gNewline();
    }

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

int find(PCBType ** process, int pid){
    listtoBeginAsc(pcbs);
    int found = 0;
    while(!found ){
        *process = listnextAsc(pcbs);
        if ((*process)->pid == pid){
            return 1;
        }
    }
    return 0;
}

