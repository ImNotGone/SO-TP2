#include <libs/process.h>

typedef struct processData{
    uint16_t pid, ppid;
    uint8_t priority;
    uint64_t rsp, stack_base, rip;
    char * status;
    int argc;
    char ** argv;
} PCB;

void newProcess(uint64_t rip, int argc, char * argv[]){
    //do something
}
