#include <commands/programs.h>


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
    char buffer[BUFF_SIZE];
    int readed;
    while (!scanf("%s", buffer)) {
        printf("%s\n", buffer);
    }
}

// reads from STDIN
// prints the amount of lines
void wc(int argc, char * argv[]) {
    uint64_t lines = 0;
    char c = 0;
    for(c = getchar(); c != EOF; c = getchar()) {
        if(c == '\n') {
            lines += 1;
        }
    }
    printf("lines: %d\n", lines);
    return;
}

// ====================== Other ======================

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
