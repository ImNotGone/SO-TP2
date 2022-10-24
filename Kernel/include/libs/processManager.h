#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <stdint.h>
#include <libs/process.h>

// C functions
uint64_t newProcess(uint64_t rip, int argc, char * argv[]);

uint64_t schedule(uint64_t rsp);

#endif //PROCESSMANAGER_H

