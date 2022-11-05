#ifndef _TESTS_COMMANDS_H
#define _TESTS_COMMANDS_H

#include <tests.h>

// Tests memory manager
void memtest();

// Tests semaphores
void synctest(int argc, char *argv[]);

// Tests priority
void priotest();
#endif
