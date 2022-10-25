// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <_stdio.h>
#include <testCommands.h>

void memtest() {
    char *argv[] = {"128"};
    int testResult = test_mm(1, argv);

    printf("Memory test %s\n", testResult == 0 ? "passed" : "failed", argv[0]);
}
