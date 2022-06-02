#include <stdint.h>
#include <dumps.h>
#include <time.h>
#include <_stdio.h>
#include <_string.h>
#include <syscalls.h>

#define COMMAND_BUFFER_SIZE 256
#define LEFT_SCREEN 0
#define FULL_SCREEN 0
#define RIGHT_SCREEN 1

typedef struct iterator {
    int started;
    int (*hasNext) (int, int);
    void (*next) (int);
    void (*reset) (int);
} iterator;

typedef void (*fp)(void);

typedef struct command {
    char * name;
    char * desc;
    fp exec;
    iterator it;
} command;

static void init();
static void command_listener();
static void help();
static void inforeg();
extern void zerodiv();
extern void invalidopcode();
static void printmem();
static void time();
static void primes();
static void fibo();
static void screenDiv(command leftCommand, command rightCommand);
static void checkExited();
static void resetExited();



// Fibonacci
typedef struct fiboInternalState {
    uint64_t i;
    uint64_t prev;
    uint64_t current;
    uint64_t next;
} fiboInternalState;

static fiboInternalState fiboInternalStates[] = {{0, 0, 1, 1}, {0, 0, 1, 1}};

static void fiboNext(int screen) {
    if (fiboInternalStates[screen].i == 0) {
        printf("fibo(%d) = %d\n", 0, 0);
        fiboInternalStates[screen].i++;
        return;
    }

    printf("fibo(%d) = %d\n", fiboInternalStates[screen].i, fiboInternalStates[screen].current);
    fiboInternalStates[screen].i++;
    fiboInternalStates[screen].prev = fiboInternalStates[screen].current;
    fiboInternalStates[screen].current = fiboInternalStates[screen].next;
    fiboInternalStates[screen].next = fiboInternalStates[screen].prev + fiboInternalStates[screen].current;
}

static int fiboHasNext(int started, int screen) {
    return fiboInternalStates[screen].current <= UINT64_MAX - fiboInternalStates[screen].prev;
}

static void fiboReset(int screen) {
    fiboInternalStates[screen].i = 0;
    fiboInternalStates[screen].prev = 0;
    fiboInternalStates[screen].current = 1;
    fiboInternalStates[screen].next = 1;
}

// Primes
typedef struct primesInternalState {
    int isPrime;
    uint64_t number;
} primeInternalState;

static primeInternalState primeInternalStates[] = {{1, 1}, {1, 1}};

static void primesNext(int screen) {
    for (uint64_t i = 2; i * i <= primeInternalStates[screen].number && primeInternalStates[screen].isPrime; i++) {
        if (primeInternalStates[screen].number % i == 0)
            primeInternalStates[screen].isPrime = 0;
    }
    if (primeInternalStates[screen].isPrime)
        printf("-- %d\n", primeInternalStates[screen].number);

    primeInternalStates[screen].isPrime = 1;
    primeInternalStates[screen].number += 2;
}

static int primesHasNext(int started, int screen) {
    return primeInternalStates[screen].number <=UINT64_MAX;
}

static void primesReset(int screen) {
    primeInternalStates[screen].isPrime = 1;
    primeInternalStates[screen].number = 1;
}

// nonIterableCommands functions
static int nonIterableCommandHasNext(int started, int screen) {
    return !started;
}

static void nonIterableCommandReset(int screen) {}




static int exited[] = {0, 0};

static command commands[] = {
        {"help", "shows all available commands", help,                                          {0, nonIterableCommandHasNext, help, nonIterableCommandReset}},
        {"inforeg", "prints register snapshot, take a snapshot using \'cntrl + s\'", inforeg,   {0, nonIterableCommandHasNext, inforeg, nonIterableCommandReset}},
        {"zerodiv", "generates a zero divition exeption", zerodiv,                              {0, nonIterableCommandHasNext, zerodiv, nonIterableCommandReset}},
        {"invalid_opcode", "generates an invalid operation exception", invalidopcode,           {0, nonIterableCommandHasNext, invalidopcode, nonIterableCommandReset}},
        {"time", "prints the current system time", time,                                        {0, nonIterableCommandHasNext, time, nonIterableCommandReset}},
        {"primes", "prints primes", primes,                                                     {0, primesHasNext, primesNext, primesReset}},
        {"fibo", "prints the fibonacci series",fibo,                                            {0, fiboHasNext, fiboNext, fiboReset}},
        {"printmem", "prints the 32 bytes which follow the recieved address", printmem,         {0, nonIterableCommandHasNext, printmem, nonIterableCommandReset}},
        {"|", "allows to divide the screen and run 2 programms", (fp) screenDiv,                {0, nonIterableCommandHasNext, (void (*) (int))screenDiv, nonIterableCommandReset}}
};

static int commandsDim = sizeof(commands)/sizeof(commands[0]);

int main() {
    init();
    while(1) {
        putchar('>');
        command_listener();
    }
}


static void init() {
    puts("Bienvenido a la consola");
    help();
}

static void command_listener() {
    char commandBuffer[COMMAND_BUFFER_SIZE];
    int c;
    int i = 0;
    while((c = getchar()) != '\n' && i < COMMAND_BUFFER_SIZE) {
        if(c == '\b') {
            if(i > 0) {
                putchar(c);
                i--;
            }
        }
        if(c != '\b') {
            putchar(c);
            commandBuffer[i++] = c;
        }
    }
    commandBuffer[i] = 0;
    putchar('\n');
    if(strcmp(commandBuffer, "") == 0) return;
    for (i = 0; i < commandsDim - 2; i++) {
        if (strcmp(commandBuffer, commands[i].name) == 0) {
            commands[i].exec();
            return;
        }
    }
    if (strncmp(commandBuffer, "printmem", 8) == 0) {
        if (commandBuffer[8] == ' ' && commandBuffer[9] != ' ') { //argumentos igual que en linux
            char argument[COMMAND_BUFFER_SIZE];
            for (i = 9; commandBuffer[i] != '\0' && i - 9 < COMMAND_BUFFER_SIZE; i++)
                argument[i - 9] = commandBuffer[i];
            const char * out;
            int64_t address = strtol(argument, &out, 16);
            if (address < 0) {
                puts("Direccion de memoria invalida");
                return;
            }
            printmem(address);
            return;
        }
        puts("Argumento invalido");
        return;
    }
    char leftStr[COMMAND_BUFFER_SIZE];
    char rightStr[COMMAND_BUFFER_SIZE];

    if (!strDivide(commandBuffer, leftStr, rightStr, '|')) {
        printf("%s -> comando invalido\n", commandBuffer);
        return;
    }

    command leftCommand, rightCommand;
    int leftFound = 0, rightFound = 0;
    for (i = 0; i < commandsDim - 1; i++) {
        if (strcmp(leftStr, commands[i].name) == 0) {
            leftCommand = commands[i];
            leftFound = 1;
        }
        if (strcmp(rightStr, commands[i].name) == 0) {
            rightFound = 1;
            rightCommand = commands[i];
        }
    }

    if (leftFound && rightFound) {
        screenDiv(leftCommand, rightCommand);
        return;
    }

    if (!leftFound)
        printf("%s -> comando invalido\n", leftStr);
    if (!rightFound)
        printf("%s -> comando invalido\n", rightStr);


}
static void help() {
    puts("Los comandos disponibles son:");
    for(int i = 0; i < commandsDim; i++) {
        printf("%d) %s => %s\n", i, commands[i].name, commands[i].desc);
    }
}
static void inforeg() {
    regDump();
}

static void printmem(uint64_t address) {
    memDump(address);
}
static void time() {
    printTime();
}
static void primes() {
    while (!exited[FULL_SCREEN] && primesHasNext(0, FULL_SCREEN)) {
        primesNext(FULL_SCREEN);
        checkExited();
    }
    primesReset(FULL_SCREEN);
    resetExited();
}
static void fibo() {
    while (!exited[FULL_SCREEN] && fiboHasNext(0, FULL_SCREEN)) {
        fiboNext(FULL_SCREEN);
        checkExited();
    }
    fiboReset(FULL_SCREEN);
    resetExited();
}

static void waitForEnter() {
    while (getchar() != '\n');
}

static void checkExited() {
    char c;
    sysread(STDIN, &c, 1);
    if (c == 's') {
        exited[LEFT_SCREEN] = 1;
        exited[RIGHT_SCREEN] = 1;
        return;
    }
    if (c == 'a')
        exited[LEFT_SCREEN] = 1;
    if (c == 'd')
        exited[RIGHT_SCREEN] = 1;
}

static void resetExited() {
    exited[LEFT_SCREEN] = 0;
    exited[RIGHT_SCREEN] = 0;
}

static void handlePipe(command leftCommand, command rightCommand) {
    sysDivWind();
    while ((!exited[LEFT_SCREEN] && leftCommand.it.hasNext(leftCommand.it.started, LEFT_SCREEN)) || (!exited[RIGHT_SCREEN] && rightCommand.it.hasNext(rightCommand.it.started, RIGHT_SCREEN))) {
        checkExited();
        sysSetWind(LEFT_SCREEN);
        // if hay q cortar programa salimos?
        if (!exited[LEFT_SCREEN] && leftCommand.it.hasNext(leftCommand.it.started, LEFT_SCREEN)) {
            leftCommand.it.next(LEFT_SCREEN);
            leftCommand.it.started = 1;
        }
        checkExited();
        sysSetWind(RIGHT_SCREEN);
        if (!exited[RIGHT_SCREEN] && rightCommand.it.hasNext(rightCommand.it.started, RIGHT_SCREEN)) {
            rightCommand.it.next(RIGHT_SCREEN);
            rightCommand.it.started = 1;
        }
    }
    leftCommand.it.reset(LEFT_SCREEN);
    rightCommand.it.reset(RIGHT_SCREEN);
    resetExited();
}

static void screenDiv(command leftCommand, command rightCommand) {
    sysDivWind();
    handlePipe(leftCommand, rightCommand);
    waitForEnter();
    sysOneWind();
}