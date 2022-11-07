// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stdio.h>
#include <syscalls.h>
#include <test_util.h>

#define MINOR_WAIT \
    500000 // TODO: Change this value to prevent a process from flooding the
           // screen
#define WAIT \
    30000000 // TODO: Change this value to make the wait long enough to see theese
             // processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 1  // TODO: Change as required
#define MEDIUM 3  // TODO: Change as required
#define HIGHEST 5 // TODO: Change as required

static void bussy_wait(uint64_t n);
static void endless_loop_print();

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {
    int64_t pids[TOTAL_PROCESSES];
    char *argv[] = {"endless_loop_print"};
    uint64_t i;

    for (i = 0; i < TOTAL_PROCESSES; i++) {
        pids[i] = syscreateprocess((uint64_t)endless_loop_print, 1, 1, 1, argv);
        sysunblock(pids[i]);
    }

    bussy_wait(WAIT);
    printf("\nCHANGING PRIORITIES...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        sysnice(pids[i], prio[i]);

    bussy_wait(WAIT);
    printf("\nBLOCKING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        sysblock(pids[i]);

    printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        sysnice(pids[i], MEDIUM);

    printf("UNBLOCKING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        sysunblock(pids[i]);

    bussy_wait(WAIT);
    printf("\nKILLING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        syskill(pids[i]);
}

// Dummies
static void bussy_wait(uint64_t n) {
    uint64_t i;
    for (i = 0; i < n; i++)
        ;
}

static void endless_loop_print() {
    int64_t pid = sysgetpid();

    while (1) {
        printf("%d ", (int)pid);
        bussy_wait(MINOR_WAIT);
    }
}
