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
        printf("  Name: %s\n", semInfo[i].name);
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


void ps() { sysps(); }

// ==================== Process Control ====================

void nice(int argc, char * argv[]){
    //pasar a dec
    const char * endptr;
    pid_t pid = strtol(argv[1], &endptr, 10);
    int priority = strtol(argv[2], &endptr, 10);

    sysnice(pid, priority);
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

    syskill(pid);
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

