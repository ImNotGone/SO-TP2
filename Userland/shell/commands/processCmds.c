#include <processCmds.h>

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
