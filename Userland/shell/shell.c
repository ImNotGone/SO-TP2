// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shell.h>
#include <stdio.h>
#define STDERR 2
#define FORE 0
#define BACK 1

// -------- main functions -------------------------------------
// static void init();
static void command_listener();

// ------------ commands implemented on this file -----------------------

//----- auxiliary functions ------------

static void help();
static void ps();
static void displayError(int64_t add, const char *command);
static int64_t parsePrintmem(char *commandBuffer);
static char **parseArgs(char *commandBuffer, int *argc, int *isBackground);


//--- Flags ---
static int64_t printmemAddresses[] = {INVALID_ADDRESS, INVALID_ADDRESS};

// -------------- Terminal commands ---------------------------------

static command commands[] = {
    {"help", "Prints this help", help},
    {"printmem", "Prints the memory info", (voidfp)memDump},
    {"memtest", "Tests the memory", (voidfp)memtest},
    {"synctest", "Tests the semaphores", (voidfp)synctest},
    {"seminfo", "Prints semaphores info", semDump},
    {"meminfo", "Prints memory manager status", memManagerDump},
    {"ps", "Prints all process' information", ps},
    {"nice", "Changes priority to pid", (voidfp)nice},
};

static int commandsDim = sizeof(commands) / sizeof(commands[0]);

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

    // Vemos si el comando matchea con alguno
    // Que no requiere un parseo especial del input
    for (i = 0; i < commandsDim; i++) {

        if (strcmp(argv[0], commands[i].name) == 0) {

            uint64_t rip = (uint64_t)commands[i].exec;

            pid_t pid = syscreateprocess(rip, isBackground ? BACK : FORE, 1, argc, argv);

            if (!isBackground) {
                syswaitpid(pid);
            }

            return;
        }
    }

    // Free arguments
    for (i = 0; i < argc; i++) {
        sysfree(argv[i]);
    }
    sysfree(argv);

    // El comando no es el pipe, queda validar que es printmem para una sola
    // pantalla

    int64_t address = parsePrintmem(commandBuffer);

    if (address >=
        0) { // El comando es printmem con una direccion valida como argumento
        printmemAddresses[FULL_SCREEN] = address;
        memDump(printmemAddresses[FULL_SCREEN]);
        printmemAddresses[FULL_SCREEN] = address;
        return;
    }

    // El comando no es printmem o el argumento recibido es invalido
    displayError(address, commandBuffer);
    return;
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
        printf("%d) %s:\n%s\n\n", i + 1, commands[i].name, commands[i].desc);
    }

    // sysexits();
}

static void ps() { sysps(); }

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

// Si commandBuffer es "printmem" sin argumentos -> retorna ARGUMENT_MISSING
// Si commandBuffer no contiene "printmem" -> retorna NOT_PRINTMEM
// Si commandBuffer es "printmem" pero el argumento que recibe es invalido ->
// retorna INVALID_ARGUMENT Si commandBuffer contiene "printmem" y el argumento
// es valido -> retorna el argumento como valor numerico El argumento de
// printmem debe estar separado por un espacio como minimo, el resto de los
// espacios son ignorados
static int64_t parsePrintmem(char *commandBuffer) {
    if (strcmp(commandBuffer, "printmem") == 0)
        return ARGUMENT_MISSING;

    if (strncmp(commandBuffer, "printmem ", 9) != 0)
        return NOT_PRINTMEM;

    // Validacion del argumento

    int i = 9;
    while (commandBuffer[i] == ' ')
        i++; // Ignoro espacios

    char argument[COMMAND_BUFFER_SIZE];

    int j;
    for (j = 0; commandBuffer[i] != '\0' && j < COMMAND_BUFFER_SIZE; i++, j++)
        argument[j] = commandBuffer[i];
    argument[j] = '\0';

    const char *out;
    int64_t address = strtol(argument, &out, 16);

    if (address < 0 || (*out != '\0' && (*out != 'h')) || out[1] != '\0')
        return INVALID_ADDRESS;

    return address;
}

// Displays Error in STDERR
static void displayError(int64_t add, const char *command) {
    switch (add) {
    case ARGUMENT_MISSING:
        fputs(STDERR, ARGUMENT_MISSING_MESSAGE);
        break;
    case INVALID_ADDRESS:
        fprintf(STDERR, INVALID_ARGUMENT_MESSAGE, command + 9);
        break;
    case NOT_PRINTMEM:
        fprintf(STDERR, INVALID_COMMAND_MESSAGE_FORMAT, command);
        break;
    default:
        return;
    }
}
