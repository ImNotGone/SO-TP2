#ifndef _SHELL_H
#define _SHELL_H

#include <stdint.h>
#include <stddef.h>
#include <commands/builtins.h>
#include <commands/programs.h>
#include <_stdio.h>
#include <_string.h>
#include <_stdbool.h>
#include <syscalls.h>

#define COMMAND_BUFFER_SIZE 256
#define MAX_ARGS 64
#define MAX_ARG_SIZE 128

#define WELCOME_MESSAGE                 "Welcome to the console"
#define INVALID_COMMAND_MESSAGE_FORMAT  "%s: command not found\n"

// To expand defined value to a string
#define VALUE_TO_STRING(s)      LITERAL_TO_STRING(s)
#define LITERAL_TO_STRING(s)    #s
#define CMD_BUFF_FORMAT         "%"VALUE_TO_STRING(COMMAND_BUFFER_SIZE)"s"

#define CONSOLE_PROMPT  "@byte: > "
#define HELP_DESC       "Shows all available commands."

// type definitions
typedef void (*commandfp)(int argc, char *argv[]);

typedef struct command {
    char * name;
    char * desc;
    commandfp exec;
} command;

#endif//_SHELL_H
