#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

uint64_t createProcess(int stackBase, int rip, int argc, char* argv[]);

#endif //PROCESS_H
