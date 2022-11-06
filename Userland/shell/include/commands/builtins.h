#ifndef _BUILTINS_H
#define _BUILTINS_H
#include <syscalls.h>
#include <_string.h>
#include <_stdio.h>

void ps();

void nice(int argc, char * argv[]);

void kill(int argc, char * argv[]);

void block(int argc, char * argv[]);

void memManagerDump();

void semDump();



#endif
