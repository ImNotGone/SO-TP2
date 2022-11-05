#ifndef _PROGRAMS_H
#define _PROGRAMS_H
#include <tests.h>
#include <syscalls.h>
#include <_stdio.h>
#include <_string.h>



// ==================== TESTS ====================

// Tests memory manager
void memtest();

// Tests semaphores
void synctest(int argc, char *argv[]);

// Tests processes
void processtest(int argc, char *argv[]);

// Tests priority
void priotest();

// ==================== OTHER ====================

void loop(int argc, char **argv);

#endif
