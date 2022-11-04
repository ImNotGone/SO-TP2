#include <processCmds.h>

void nice(int argc, char * argv[]){
    //pasar a dec
    const char * endptr;
    pid_t pid = strtol(argv[1], &endptr, 10);
    int priority = strtol(argv[2], &endptr, 10);

    sysnice(pid, priority);
}
