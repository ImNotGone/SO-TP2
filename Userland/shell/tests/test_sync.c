// // This is a personal academic project. Dear PVS-Studio, please check it.
// // PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// #include <syscalls.h>
// #include "test_util.h"
// #include <stdint.h>
// #include <stdio.h>
//
// #define SEM_ID "sem"
// #define TOTAL_PAIR_PROCESSES 2
//
// int64_t global;  //shared memory
//
// void slowInc(int64_t *p, int64_t inc) {
//     uint64_t aux = *p;
//     sysyield(); // This makes the race condition highly probable
//     aux += inc;
//     *p = aux;
// }
//
// uint64_t my_process_inc(uint64_t argc, char *argv[]) {
//     uint64_t n;
//     int8_t inc;
//     int8_t use_sem;
//
//     sem_t sem;
//
//     if (argc != 3)
//         return -1;
//
//     if ((n = satoi(argv[0])) <= 0)
//         return -1;
//     if ((inc = satoi(argv[1])) == 0)
//         return -1;
//     if ((use_sem = satoi(argv[2])) < 0)
//         return -1;
//
//     if (use_sem) {
//         sem = syssemopen(SEM_ID, 1);
//         if (!sem) {
//             printf("test_sync: ERROR opening semaphore\n");
//             return -1;
//         }
//     }
//
//     uint64_t i;
//     for (i = 0; i < n; i++) {
//         if (use_sem)
//             syssemwait(sem);
//         slowInc(&global, inc);
//         if (use_sem)
//             syssempost(sem);
//     }
//
//     if (use_sem)
//         syssemclose(sem);
//
//     return 0;
// }
//
// uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
//     uint64_t pids[2 * TOTAL_PAIR_PROCESSES];
//
//     if (argc != 2)
//         return -1;
//
//     char *argvDec[] = {argv[0], "-1", argv[1], NULL};
//     char *argvInc[] = {argv[0], "1", argv[1], NULL};
//
//     global = 0;
//
//     uint64_t i;
//     for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
//         pids[i] = my_create_process("my_process_inc", 3, argvDec);
//         pids[i + TOTAL_PAIR_PROCESSES] =
//             my_create_process("my_process_inc", 3, argvInc);
//     }
//
//     for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
//         my_wait(pids[i]);
//         my_wait(pids[i + TOTAL_PAIR_PROCESSES]);
//     }
//
//     printf("Final value: %d\n", global);
//
//     return 0;
// }
