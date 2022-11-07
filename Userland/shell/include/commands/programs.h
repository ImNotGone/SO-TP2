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

// Tests named pipes
void fifotest(int argc, char * argv[]);

// ==================== PIPES ====================

// Prints stdin to stdout
void cat(int argc, char *argv[]);

// Counts the number of lines in stdin
void wc(int argc, char * argv[]);

// Filters out vowels
void filter(int argc, char * argv[]);

// ==================== OTHER ====================

void loop(int argc, char **argv);

void phylo(int argc, char **argv);

#endif
