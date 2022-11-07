// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <commands/builtins.h>

// ==================== Dumps ====================

void memManagerDump(int argc, char *argv[]) {

    // Get memory info
    TMemInfo memInfo;
    sysmeminfo(&memInfo);

    // Print memory memory info
    printf("Total memory: %d bytes (%d MB)\n", memInfo.total, memInfo.total / 1024 / 1024);
    printf("Used memory: %d bytes (%d MB)\n", memInfo.used, memInfo.used / 1024 / 1024);
    printf("Free memory: %d bytes (%d MB)\n", memInfo.free, memInfo.free / 1024 / 1024);
    //sysexits();

}

void semDump(int argc, char *argv[]) {

    // Get semaphore info
    uint64_t semAmount;
    TSemInfo *semInfo = sysseminfo(&semAmount);

    if (semAmount == 0) {
        puts("No semaphores found");
        return;
    }

    // Print semaphores info
    printf("There are %d semaphores:\n", semAmount);
    for (int i = 0; i < semAmount; i++) {

        printf("Semaphore %d:\n", i);
        printf("  Name: %s\n", semInfo[i].name == 0 ? "unnamed" : semInfo[i].name);
        printf("  Value: %d\n", semInfo[i].value);

        // Print waiting queue
        printf("  There are %d waiting processes\n", semInfo[i].waitingQueueSize);
        for (int j = 0; j < semInfo[i].waitingQueueSize; j++) {
            printf("    PID: %d\n", semInfo[i].waitingQueue[j]);
        }
    }

    // Free memory
    for (int i = 0; i < semAmount; i++) {
        sysfree(semInfo[i].waitingQueue);
    }
    sysfree(semInfo);
}

static void printProcInfo(TProcInfo proc) {

    putchar('\n');
    printf("NAME: %s\n", proc.name == 0 ? "unnamed" : proc.name);

    printf("PID: %d\n", proc.pid);

    printf("Priority: %d | Stack base: %x | Stack pointer: %x | Ground: %s | Status: %s\n", proc.priority, proc.stackBase, proc.rsp, proc.ground == 0 ? "foreground" : "background", proc.status == 0 ? "Ready" : proc.status == 1 ? "Blocked" : proc.status == 2 ? "Killed" : "Sleeping");


    if(proc.waitingProcessesSize > 0) {
        printf("Waiting on this process are (pids): ");
        printf("{");
        for (uint64_t i = 0; i < proc.waitingProcessesSize; i++) {
            printf(" %d ", proc.waitingProcesses[i]);
        }
        printf("}\n");
    }
}

void ps() {
    uint64_t procAmount;
    TProcInfo *procInfo = sysps(&procAmount);

    if (procAmount == 0 || procInfo == 0) {
        puts("No processes found");
        return;
    }

    printf("There are %d processes:\n", procAmount);

    for (int i = 0; i < procAmount; i++) {
        printProcInfo(procInfo[i]);
    }

    // Free memory
    for (int i = 0; i < procAmount; i++) {
        sysfree(procInfo[i].waitingProcesses);
        sysfree(procInfo[i].name);
    }

    sysfree(procInfo);
}

void pipeDump() {

        uint64_t pipeAmount;
        TPipeInfo *pipeInfo = syspipeinfo(&pipeAmount);

        if (pipeAmount == 0 || pipeInfo == 0) {
            puts("No pipes found");
            return;
        }

        printf("There are %d pipes:\n", pipeAmount);

        for (int i = 0; i < pipeAmount; i++) {
            printf("Pipe %d:\n", i);
            printf("  Name: %s\n", pipeInfo[i].name == 0 ? "unnamed" : pipeInfo[i].name);
            printf("  Size: %d\n", pipeInfo[i].size);
            printf("  Reader Offset: %d\n", pipeInfo[i].readerOffset);
            printf("  Writer Offset: %d\n", pipeInfo[i].writerOffset);

            // Print waiting queue
            printf("  There are %d waiting processes\n", pipeInfo[i].waitingProcessesSize);
            for (int j = 0; j < pipeInfo[i].waitingProcessesSize; j++) {
                printf("    PID: %d\n", pipeInfo[i].waitingProcesses[j]);
            }
        }

        // Free memory
        for (int i = 0; i < pipeAmount; i++) {
            sysfree(pipeInfo[i].waitingProcesses);
        }

        sysfree(pipeInfo);
}

// ==================== Process Control ====================

void nice(int argc, char * argv[]){

    if(argc != 3){
        fprintf(STDERR,"Usage: nice <pid> <priority>[1-10]\n");
        return;
    }

    //pasar a dec
    const char * endptr;
    pid_t pid = strtol(argv[1], &endptr, 10);
    int priority = strtol(argv[2], &endptr, 10);

    if (pid < 1) {
        fprintf(STDERR, "Invalid pid\n");
        return;
    }

    if(priority < 1 || priority > 10){
        fprintf(STDERR, "1 <= priority <= 10\n");
        return;
    }

    if(sysnice(pid, priority)<0){
        fprintf(STDERR, "Error changing priority\n");
    }
}

void kill(int argc, char * argv[]){

    if (argc != 2) {
        fprintf(STDERR, "Usage: kill <pid>\n");
        return;
    }

    // Get pid
    const char * endptr;
    pid_t pid = strtol(argv[1], &endptr, 10);

    if (pid < 2) {
        fprintf(STDERR, "Invalid pid\n");
        return;
    }

     if (syskill(pid) == -1) {
        fprintf(STDERR, "Process not found\n");
        return;
    }

    printf("[%d] Killed\n", pid);
}

void block(int argc, char * argv[]){
    if (argc != 2) {
        fprintf(STDERR, "Usage: block <pid>\n");
        return;
    }

    // Get pid
    const char * endptr;
    pid_t pid = strtol(argv[1], &endptr, 10);

    if (pid < 2) {
        fprintf(STDERR, "Invalid pid\n");
        return;
    }

    int blockResult = sysblock(pid);
    int unblockResult = -1;

    // Maybe process is already blocked
    if (blockResult == -1) {
        unblockResult = sysunblock(pid);
    }

    if (blockResult == -1 && unblockResult == -1) {
        fprintf(STDERR, "Process %d not found\n", pid);
    }
}

