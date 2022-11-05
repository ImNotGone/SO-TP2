#ifndef _TESTS_H
#define _TESTS_H

#include <test_util.h>

uint64_t test_mm(uint64_t argc, char *argv[]);

uint64_t test_sync(uint64_t argc, char *argv[]); //{n, use_sem, 0}

void test_prio();
#endif
