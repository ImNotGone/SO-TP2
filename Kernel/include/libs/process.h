#ifndef PROCESS_H
#define PROCESS_H

#include <types.h>

extern uint64_t createProcess(int stackBase, int rip, int argc, char* argv[], bool isFork);

extern uint64_t execProcess(int stackBase, int rip, int argc, char* argv[]);

#endif //PROCESS_H
