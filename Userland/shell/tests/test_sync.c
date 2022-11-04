// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "test_util.h"
#include <_string.h>
#include <dumps.h>
#include <stdint.h>
#include <stdio.h>
#include <syscalls.h>

#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 2

int global; // shared memory

static void slowInc(int *p, int64_t inc) {
    uint64_t aux = *p;
    sysyield(); // This makes the race condition highly probable
    aux += inc;
    *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
    uint64_t n;
    int inc;
    int8_t use_sem;

    sem_t sem;

    if (argc != 3)
        return -1;

    if ((n = satoi(argv[0])) <= 0)
        return -1;

    if ((inc = satoi(argv[1])) == 0)
        return -1;

    if ((use_sem = satoi(argv[2])) < 0)
        return -1;

    if (use_sem) {
        sem = syssemopen(SEM_ID, 1);
        if (sem == -1) {
            printf("test_sync: ERROR opening semaphore\n");
            return -1;
        }
    }

    int i;
    for (i = 0; i < n; i++) {

        if (use_sem) {
            syssemwait(sem);
        }

        slowInc(&global, inc);

        if (use_sem) {
            syssempost(sem);
        }
    }

    printf("Process %d finished\n", (int)sysgetpid());

    if (use_sem)
        syssemclose(sem);

    return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
    pid_t pids[2 * TOTAL_PAIR_PROCESSES];

    if (argc != 2) {
        return -1;
    }

    char *argvDec[] = {argv[0], "-1", argv[1], 0};
    char *argvInc[] = {argv[0], "1", argv[1], 0};

    global = 0;

    uint64_t i;
    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        pids[i] = syscreateprocess((uint64_t)my_process_inc, 1, 2, 3, argvDec);

        printf("Created process %d\n", (int)pids[i]);

        pids[i + TOTAL_PAIR_PROCESSES] =
            syscreateprocess((uint64_t)my_process_inc, 1, 2, 3, argvInc);

        printf("Created process %d\n", (int)pids[i + TOTAL_PAIR_PROCESSES]);
    }

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        syswaitpid(pids[i]);
        syswaitpid(pids[i + TOTAL_PAIR_PROCESSES]);
    }

    printf("Final value: %d\n", global);

    return global;
}
