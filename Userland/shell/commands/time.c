// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <time.h>
#include <syscalls.h>

void printTime() {
    TTime t;
    systime(&t);
    printf("Today is: %d/%d/%d\nThe actual time is: %dhs %dm %ds\n", t.day, t.month, t.year, t.hour, t.min, t.sec);
}