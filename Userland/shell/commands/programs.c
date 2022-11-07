// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "_stdio.h"
#include <commands/programs.h>

#define IS_VOWEL(c) ((c) == 'a' || (c) == 'e' || (c) == 'i' || (c) == 'o' || (c) == 'u' || \
                    (c) == 'A' || (c) == 'E' || (c) == 'I' || (c) == 'O' || (c) == 'U')

#define INITIAL_PHYLOS 5

// ====================== Tests ======================

void memtest(int argc, char * argv[]) {

    if (argc != 2) {
        fprintf(STDERR, "Usage: %s <size>Mb\n", argv[0]);
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

    printf("Memory test %s\nTested using %sMb\n",
           testResult == 0 ? "passed" : "failed", argv[1]);
}

void synctest(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(STDERR,
                "Usage: test_sync <n> <use_sem>[named | unnamed | no-sem]\n");
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
        fprintf(STDERR, "Usage: test_process <number_of_process>\n");
        return;
    }

    if (satoi(argv[1]) <= 0) {
        fprintf(STDERR, "Invalid number of process, it must be greater than 0\n");
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
    while ((c = getchar(STDIN)) != -1) {
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

// ====================== Other ======================

// Prints a greeting and then sleeps for n seconds, for eternity
void loop(int argc, char **argv) {

    if (argc != 2) {
        fprintf(STDERR, "Usage: loop <number of seconds>\n");
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

