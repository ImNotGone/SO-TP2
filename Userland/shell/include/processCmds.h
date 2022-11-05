#ifndef PROCESSCMDS_H
#define PROCESSCMDS_H

#include <syscalls.h>
#include <_string.h>
#include <_stdio.h>

void nice(int argc, char * argv[]);

void kill(int argc, char * argv[]);

void block(int argc, char * argv[]);

#endif // PROCESSCMDS_H
