// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <_stdio.h>
#include <_string.h>
#include <testCommands.h>

void memtest() {
    char *argv[] = {"128"};
    int testResult = test_mm(1, argv);

    printf("Memory test %s\nTested using %sMb\n",
           testResult == 0 ? "passed" : "failed", argv[0]);
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

void priotest() {
    test_prio();
}
