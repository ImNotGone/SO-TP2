#ifndef PROCESS_H
#define PROCESS_H

#include <types.h>

extern uint64_t createProcess(int stackBase, int rip, int argc, char* argv[], bool isFork);

#endif //PROCESS_H
