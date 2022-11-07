// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "_stdio.h"
#include <commands/programs.h>

#define IS_VOWEL(c) ((c) == 'a' || (c) == 'e' || (c) == 'i' || (c) == 'o' || (c) == 'u' || \
                    (c) == 'A' || (c) == 'E' || (c) == 'I' || (c) == 'O' || (c) == 'U')

#define INITIAL_PHYLOS 5

#define PIPE_NAME "pipe"
#define PIPE_WRITERS 5
#define PIPE_WRITERS_ITERATIONS 100

// ====================== Tests ======================

void memtest(int argc, char * argv[]) {

    if (argc != 2) {
        fprintf(STDERR, "Usage: %s <size>MB\n", argv[0]);
        return;
    }

    // Get size
    const char * endptr;
    int size = strtol(argv[1], &endptr, 10);

    if (size <= 0) {
        fprintf(STDERR, "Invalid size\n");
        return;
    }

    char * testParams[] = { argv[1] };

    int testResult = test_mm(1, testParams);

    printf("Memory test %s\nTested using %sMB\n",
           testResult == 0 ? "passed" : "failed", argv[1]);
}

void synctest(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(STDERR,
                "Usage: synctest <n> <use_sem>[named || unnamed || no-sem]\n");
        return;
    }

    if (satoi(argv[1]) <= 0) {
        fprintf(STDERR,
                "Invalid number of iterations, it must be greater than 0\n");
        return;
    }

    if (strcmp(argv[2], "named") != 0 && strcmp(argv[2], "unnamed") != 0 &&
        strcmp(argv[2], "no-sem") != 0) {
        fprintf(STDERR,
                "Invalid use_sem value it must be named, unnamed or no-sem\n");
        return;
    }

    char *testParams[] = {argv[1], argv[2]};

    int testResult = test_sync(2, testParams);

    printf("Sync test %s\n", testResult == 0 ? "passed" : "failed");
}

void processtest(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(STDERR, "Usage: proctest <number_of_processes>\n");
        return;
    }

    if (satoi(argv[1]) <= 0) {
        fprintf(STDERR, "Number of processes must be greater than 0\n");
        return;
    }

    char *testParams[] = {argv[1]};

    int testResult = test_processes(1, testParams);

    printf("Process test %s\n", testResult == 0 ? "passed" : "failed");
}

void priotest(int argc, char *argv[]) {
    test_prio();
}

// ====================== Pipes ======================

void cat(int argc, char *argv[]) {

    if (argc != 1) {
        fprintf(STDERR, "Usage: cat\n");
        return;
    }

    char c;

    // Use getchar
    while ((c = getchar(STDIN)) != EOF) {
        putchar(c);
    }
}

// reads from STDIN
// prints the amount of lines
void wc(int argc, char * argv[]) {

    if (argc != 1) {
        fprintf(STDERR, "Usage: wc\n");
        return;
    }

    uint64_t lines = 0;
    uint64_t words = 0;
    uint64_t bytes = 0;
    char c = 0;
    char ant = 0;
    for(c = getchar(); (int) c != EOF; c = getchar()) {
        if(c == '\n') {
            lines++;
            words++;
        }
        if(c == ' ') {
            words++;
        }
        bytes++;
        putchar(c);
        ant = c;
    }

    if(ant != '\n') {
        words++;
    }

    printf("      %d      %d      %d\n", lines, words, bytes);
    return;
}


// Reads from STDIN
// Filters out all the vowels, and prints the rest
void filter(int argc, char * argv[]) {

    if (argc != 1) {
        fprintf(STDERR, "Usage: filter\n");
        return;
    }

    char c = 0;
    for(c = getchar(); (int) c != EOF; c = getchar()) {
        if(!IS_VOWEL(c)) {
            putchar(c);
        }
    }
    return;
}

// Writes to named pipe
static void pipeWriter(int argc, char * argv[]) {

    if (argc != 2) {
        return;
    }

    fd_t pipe_fd[2];
    if (sysmkfifo(PIPE_NAME, pipe_fd) != 0) {
        fprintf(STDERR, "Error creating pipe\n");
        return;
    }

    // Close read end
    if (sysclose(pipe_fd[0]) != 0) {
        fprintf(STDERR, "Error closing pipe\n");
        return;
    }

    for (int i = 0; i < PIPE_WRITERS_ITERATIONS; i++) {
        syswrite(pipe_fd[1], argv[1], strlen(argv[1]));
        sysyield();
    }


    // Close write end
    if (sysclose(pipe_fd[1]) != 0) {
        fprintf(STDERR, "Error closing pipe\n");
        return;
    }
}

// Demostates the use of named pipes
// Creates a named pipe, and then creates 5 child processes that will write to it
// The parent process will read from the pipe and print the result

void fifotest(int argc, char * argv[]) {

    if (argc != 1) {
        fprintf(STDERR, "Usage: fifotest\n");
        return;
    }

    fd_t pipe_fd[2];

    if (sysmkfifo(PIPE_NAME, pipe_fd) != 0) {
        fprintf(STDERR, "Error creating pipe\n");
        return;
    }


    pid_t pids[INITIAL_PHYLOS];

    int i;
    char * child_letter[] = {"A", "B", "C", "D", "E"};
    for (i = 0; i < PIPE_WRITERS; i++) {
        char * child_argv[] = { "fifotestChild" , child_letter[i] };


        pids[i] = syscreateprocess((uint64_t) pipeWriter, 1, 1, 2, child_argv);

        sysunblock(pids[i]);
    }



    // Read from pipe
    char c;
    int read;
    for (read = 0; read < PIPE_WRITERS * PIPE_WRITERS_ITERATIONS; read++) {
        sysread(pipe_fd[0], &c, 1);
        putchar(c);
    }

    // Wait for all children to finish
    for (i = 0; i < INITIAL_PHYLOS; i++) {
        syswaitpid(pids[i]);
    }

    // Close write end of pipe
    if (sysclose(pipe_fd[1]) != 0) {
        fprintf(STDERR, "Error closing pipe\n");
        return;
    }

    // Close read end of pipe
    if (sysclose(pipe_fd[0]) != 0) {
        fprintf(STDERR, "Error closing pipe\n");
        return;
    }

    // Delete pipe
    if (sysunlink(PIPE_NAME) != 0) {
        fprintf(STDERR, "Error deleting pipe\n");
        return;
    }

    printf("\nFifo test %s read %d must read %d\n", read == PIPE_WRITERS * PIPE_WRITERS_ITERATIONS ? "passed" : "failed", read, PIPE_WRITERS * PIPE_WRITERS_ITERATIONS);
}
// ====================== Other ======================

// Prints a greeting and then sleeps for n seconds, for eternity
void loop(int argc, char **argv) {

    if (argc != 2) {
        fprintf(STDERR, "Usage: loop <sleep_seconds>\n");
        return;
    }

    const char * aux;

    int seconds = strtol(argv[1], &aux, 10);

    if (*aux != '\0' || seconds < 0) {
        fprintf(STDERR, "Invalid number of iterations\n");
        return;
    }


    while (1) {

        // Print a greeting

        printf("Hello, world!, I'm a process with PID %d\n", sysgetpid());

        // Sleep for n seconds
        if (seconds > 0) {
            syssleep(sysgetpid(), seconds);
        }
    }
}

