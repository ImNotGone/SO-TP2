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
static char **parseArgs(char *commandBuffer, int *argc, int *isBackground);
static void freeArgs(char **argv, int argc);
static int64_t makePipe(commandfp leftProgram, commandfp rightProgram, int leftIsBackground, int rightIsBackground, int leftArgc, int rightArgc, char **leftArgv, char **rightArgv);


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
    {"loop", "Prints a greeting and goes to sleep for 3 seconds", (commandfp) loop},
    {"cat", "Prints stdin to stdout", (commandfp)cat}
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

    // Parse for pipes
    char *leftCommand = commandBuffer;
    char *rightCommand = NULL;

    // Left command
    char **leftArgs;
    int leftArgc;
    int leftIsBackground;
    int leftProgramIndex = -1;

    // Right command
    char **rightArgs;
    int rightArgc;
    int rightIsBackground;
    int rightProgramIndex =-1;

    char * pipeLocation = strchr(commandBuffer, '|');

    // If there is a pipe, split the command in two
    if (pipeLocation != NULL) {
        *pipeLocation = '\0';
        rightCommand = pipeLocation + 1;
        
        // Parse right command
        rightArgs = parseArgs(rightCommand, &rightArgc, &rightIsBackground);
    }

    // Parse left command for arguments
    leftArgs = parseArgs(leftCommand, &leftArgc, &leftIsBackground);

    
    // Check if there is a builtin command
    for (i = 0; i < builtinsDim; i++) {

        // If we dont have a pipe execute the command
        if (strcmp(leftArgs[0], builtins[i].name) == 0 && rightCommand == NULL) {
            builtins[i].exec(leftArgc, leftArgs);

            freeArgs(leftArgs, leftArgc);
            return;
        }

        // If we have a pipe and there is a builtin, print error
        if (rightCommand != NULL && (strcmp(rightArgs[0], builtins[i].name) == 0 || strcmp(leftArgs[0], builtins[i].name) == 0)) {
            fprintf(STDERR, "Pipes are not supported for builtin commands\n");

            freeArgs(leftArgs, leftArgc);
            freeArgs(rightArgs, rightArgc);
            return;
        }
    }

    // Check if it there is a program
    for (i = 0; i < commandsDim; i++) {

        // If we dont have a pipe execute the program
        if (strcmp(leftArgs[0], programs[i].name) == 0 && rightCommand == NULL) {

            uint64_t rip = (uint64_t)programs[i].exec;

            pid_t pid = syscreateprocess(rip, leftIsBackground ? BACK : FORE, 1, leftArgc, leftArgs);
            sysunblock(pid);
            
            if (!leftIsBackground) {
                syswaitpid(pid);
            }

            freeArgs(leftArgs, leftArgc);

            return;
        }

        // If we have a pipe and we found the program, save the index
        if (rightCommand != NULL && strcmp(leftArgs[0], programs[i].name) == 0) {

            leftProgramIndex = i;
        } 
        if (rightCommand != NULL && strcmp(rightArgs[0], programs[i].name) == 0) {

            rightProgramIndex = i;
        }
    }

    // If we have a pipe and we found both programs, execute them
    if (rightCommand != NULL && leftProgramIndex != -1 && rightProgramIndex != -1) {

        if (makePipe(programs[leftProgramIndex].exec, programs[rightProgramIndex].exec, leftIsBackground, rightIsBackground, leftArgc, rightArgc, leftArgs, rightArgs) == -1) {
            fprintf(STDERR, "Error creating pipe between programs\n");
        }

        freeArgs(leftArgs, leftArgc);
        freeArgs(rightArgs, rightArgc);
        return;
    }

    // There was no command found
    
    // Left command
    if (leftProgramIndex == -1) {
        fprintf(STDERR, "Command not found: %s \n", leftArgs[0]);
    }

    // Right command
    if (rightCommand != NULL && rightProgramIndex == -1) {
        fprintf(STDERR, "Command not found: %s \n", rightArgs[0]);
    }

    // Free arguments
    freeArgs(leftArgs, leftArgc);
    if (rightCommand != NULL) {
        freeArgs(rightArgs, rightArgc);
    }
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

// Free arguments
static void freeArgs(char **argv, int argc) {
    for (int i = 0; i < argc; i++) {
        sysfree(argv[i]);
    }
    sysfree(argv);
}

// Finds each argument in the commandBuffer, and puts it in a new array
static char **parseArgs(char *commandBuffer, int *argc, int *isBackground) {
    int argsDim = 0;
    char **args = sysmalloc(MAX_ARGS * sizeof(char *));

    // Parse the commandBuffer
    for (int i = 0; commandBuffer[i] != '\0';) {

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

// Makes a pipe between two programs
static int64_t makePipe(commandfp leftProgram, commandfp rightProgram, int leftIsBackground, int rightIsBackground, int leftArgc, int rightArgc, char **leftArgs, char **rightArgs) {
    
    // Create pipe
    fd_t pipefd[2];
    if (syspipe(pipefd) == -1) {
        fprintf(STDERR, "Error creating pipe\n");
        return -1;
    }

    // Create left program
    uint64_t rip = (uint64_t)leftProgram;
    pid_t leftPid = syscreateprocess(rip, leftIsBackground ? BACK : FORE, 1, leftArgc, leftArgs);

    // Create right program
    rip = (uint64_t)rightProgram;
    pid_t rightPid = syscreateprocess(rip, rightIsBackground ? BACK : FORE, 1, rightArgc, rightArgs);

    // Connect pipe
    if (sysdup(leftPid, STDOUT, pipefd[1]) == -1) {
        fprintf(STDERR, "Error connecting left program STDOUT to pipe write\n");

        syskill(leftPid);
        syskill(rightPid);

        return -1;
    }

    if (sysdup(rightPid, STDIN, pipefd[0]) == -1) {
        fprintf(STDERR, "Error connecting right program STDIN to pipe read\n");
        syskill(leftPid);
        syskill(rightPid);
        return -1;
    }

    // Run programs
    if (sysunblock(leftPid) == -1) {
        fprintf(STDERR, "Error unblocking left program\n");
        syskill(leftPid);
        syskill(rightPid);
        return -1;
    }

    if (sysunblock(rightPid) == -1) {
        fprintf(STDERR, "Error unblocking right program\n");
        syskill(leftPid);
        syskill(rightPid);
        return -1;
    }

    // Wait for left program to finish
    if (!leftIsBackground) {
        syswaitpid(leftPid);
    }

    // Wait for right program to finish
    if (!rightIsBackground) {
        syswaitpid(rightPid);
    }

    return 0;
 
}
