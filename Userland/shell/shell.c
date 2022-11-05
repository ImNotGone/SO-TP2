// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shell.h>
#define STDERR 2
#define FORE 0
#define BACK 1

// -------- main functions -------------------------------------
// static void init();
static void command_listener();

// ------------ commands implemented on this file -----------------------

//----- auxiliary functions ------------

static void help();
static void displayError(int64_t add, const char *command);
static int64_t parsePrintmem(char *commandBuffer);
static char **parseArgs(char *commandBuffer, int *argc, int *isBackground);


//--- Flags ---
static int64_t printmemAddresses[] = {INVALID_ADDRESS, INVALID_ADDRESS};

// -------------- Terminal commands ---------------------------------

static command builtins[] = {
    {"help", "Prints this help", help},
    {"mem", "Prints memory manager status", memManagerDump},
    {"ps", "Prints all process' information", ps},
    {"sem", "Prints semaphores info", semDump},
    {"kill", "Kills a process given the pid", (commandfp)kill},
    {"nice", "Changes priority to pid", (commandfp)nice},
    {"block", "Toggles a process between blocked and unblocked goben the pid", (commandfp)block},
};

static command programs[] = {
    {"memtest", "Tests the memory", (commandfp)memtest},
    {"synctest", "Tests the semaphores", (commandfp)synctest},
    {"proctest", "Tests processes", (commandfp)processtest},
    {"priotest", "Test priority", (commandfp)priotest},
    {"loop", "Prints a greeting and goes to sleep for 3 seconds", (commandfp) loop}
};

static int commandsDim = sizeof(programs) / sizeof(programs[0]);
static int builtinsDim = sizeof(builtins) / sizeof(builtins[0]);

//--------------------- Main functions ----------------

int main() {
    while (TRUE) {
        printf(CONSOLE_PROMPT);
        command_listener();
    }
}

static void command_listener() {
    char commandBuffer[COMMAND_BUFFER_SIZE];
    int i;
    scanf(CMD_BUFF_FORMAT, commandBuffer);

    if (strcmp(commandBuffer, "") == 0)
        return;

    int argc;
    int isBackground;
    char **argv = parseArgs(commandBuffer, &argc, &isBackground);


    // Check if it is a builtin command
    for (i = 0; i < builtinsDim; i++) {
        if (strcmp(argv[0], builtins[i].name) == 0) {
            builtins[i].exec(argc, argv);
            return;
        }
    }

    // Vemos si el comando matchea con alguno
    // Que no requiere un parseo especial del input
    for (i = 0; i < commandsDim; i++) {

        if (strcmp(argv[0], programs[i].name) == 0) {

            uint64_t rip = (uint64_t)programs[i].exec;

            pid_t pid = syscreateprocess(rip, isBackground ? BACK : FORE, 1, argc, argv);
            sysunblock(pid);
            
            if (!isBackground) {
                syswaitpid(pid);
            }
        }
    }

    // Free arguments
    for (i = 0; i < argc; i++) {
        sysfree(argv[i]);
    }
    sysfree(argv);

}

//------------------- commands implemented in this file ---------------

static void help() {
    puts("\n=== GENERAL INFO ===\n");
    puts("\'s\' stops iterable or pipe programs");
    puts("\'a\' pauses left side or individual programs");
    puts("\'d\' pauses right side programs");
    puts("\n====================\n");
    puts("The available commands are:\n");
    for (int i = 0; i < commandsDim; i++) {
        printf("%d) %s:\n%s\n\n", i + 1, programs[i].name, programs[i].desc);
    }

}


//----------------- Auxiliary functions
//-------------------------------------------

// Finds each argument in the commandBuffer, and puts it in a new array
static char **parseArgs(char *commandBuffer, int *argc, int *isBackground) {
    int argsDim = 0;
    char **args = sysmalloc(MAX_ARGS * sizeof(char *));

    // Parse the commandBuffer
    for (int i = 0; commandBuffer[i] != '\0'; i++) {

        // Skip spaces
        while (commandBuffer[i] == ' ' && commandBuffer[i] != '\0') {
            i++;
        }

        // If we reached the end of the string, break
        if (commandBuffer[i] == '\0') {
            break;
        }

        // If we found an argument, copy it to a new array
        args[argsDim] = sysmalloc(MAX_ARG_SIZE * sizeof(char));
        int j = 0;
        while (commandBuffer[i] != ' ' && commandBuffer[i] != '\0') {
            args[argsDim][j] = commandBuffer[i];
            i++;
            j++;
        }
        args[argsDim][j] = '\0';
        args[argsDim] = sysrealloc(args[argsDim], (j + 1) * sizeof(char));
        argsDim++;
    }

    // If the last argument is an ampersand, remove it and set flag
    if (strcmp(args[argsDim - 1], "&") == 0) {
        sysfree(args[argsDim - 1]);
        args[argsDim - 1] = NULL;
        argsDim--;
        *isBackground = TRUE;
    } else {
        *isBackground = FALSE;
    }

    // If we found no arguments, free the array
    // Else reallocate the array to the correct size
    if (argsDim == 0) {
        sysfree(args);
        args = NULL;
    } else {
        args = sysrealloc(args, argsDim * sizeof(char *));
    }

    *argc = argsDim;
    return args;
}
