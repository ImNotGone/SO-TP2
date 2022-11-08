// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <test_util.h>
#include <_string.h>
#include <stdint.h>
#include <stdio.h>
#include <syscalls.h>

#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 2

sem_t unnamed_sem;

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

    if (argc != 4)
        return -1;

    if ((n = satoi(argv[1])) <= 0)
        return -1;

    if ((inc = satoi(argv[2])) == 0)
        return -1;

    if ((use_sem = satoi(argv[3])) < 0)
        return -1;

    if (use_sem == 1) {
        sem = syssemopen(SEM_ID, 1);
        if (sem == -1) {
            printf("test_sync: ERROR opening semaphore\n");
            return -1;
        }
    } else if (use_sem == 2) {
        sem = unnamed_sem;
    }

    int i;
    for (i = 0; i < n; i++) {

        if (use_sem) {
            if (syssemwait(sem) == -1) {
                printf("test_sync: ERROR waiting semaphore\n");
                return -1;
            }
        }

        slowInc(&global, inc);

        if (use_sem) {
            if (syssempost(sem) == -1) {
                printf("test_sync: ERROR posting semaphore\n");
            }
        }
    }

    printf("Process %d finished\n", (int)sysgetpid());

    if (use_sem == 1) {
        if (syssemclose(sem) == -1) {
            printf("test_sync: ERROR closing named semaphore\n");
            return -1;
        }
    }

    return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
    pid_t pids[2 * TOTAL_PAIR_PROCESSES];

    if (argc < 2) {
        return -1;
    }

    if (strcmp(argv[1], "unnamed") == 0) {
        unnamed_sem = sysseminit(1);
        if (unnamed_sem == -1) {
            printf("test_sync: ERROR opening unnamed semaphore\n");
            return -1;
        }
    }

    char *semType = strcmp(argv[1], "unnamed") == 0 ? "2"
                    : strcmp(argv[1], "named") == 0 ? "1"
                                                    : "0";

    char *argvDec[] = {"semaphore_decrementor", argv[0], "-1", semType, 0};
    char *argvInc[] = {"semaphore_incrementor", argv[0], "1", semType,  0};

    printf("Starting test_sync with %s semaphore and %s iterations per process\n",argv[1], argv[0]);

    global = 0;

    uint64_t i;
    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        pids[i] = syscreateprocess((uint64_t)my_process_inc, 1, 2, 4, argvDec);
        sysunblock(pids[i]);

        printf("Created process %d\n", (int)pids[i]);

        pids[i + TOTAL_PAIR_PROCESSES] = syscreateprocess((uint64_t)my_process_inc, 1, 2, 4, argvInc);
        sysunblock(pids[i + TOTAL_PAIR_PROCESSES]);

        printf("Created process %d\n", (int)pids[i + TOTAL_PAIR_PROCESSES]);
    }

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        syswaitpid(pids[i]);
        syswaitpid(pids[i + TOTAL_PAIR_PROCESSES]);
    }

    if (strcmp(argv[1], "unnamed") == 0) {
        if (syssemdestroy(unnamed_sem) == -1) {
            printf("test_sync: ERROR closing unnamed semaphore\n");
            return -1;
        }
    } else if (strcmp(argv[1], "named") == 0) {
        if (syssemunlink(SEM_ID) == -1) {
            printf("test_sync: ERROR unlinking named semaphore\n");
            return -1;
        }
    }

    printf("Final value: %d\n", global);

    return global;
}
