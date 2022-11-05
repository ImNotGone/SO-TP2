#include <loop.h>

void loop(int argc, char **argv) {
    
    if (argc != 2) {
        fprintf(STDERR, "Usage: loop <number of seconds>\n");
        return;
    }

    const char * aux;

    int seconds = strtol(argv[1], &aux, 10);

    if (*aux != '\0' || seconds <= 0) {
        fprintf(STDERR, "Invalid number of iterations\n");
        return;
    }


    while (1) {
        
        // Print a greeting

        printf("Hello, world!, I'm a process with PID %d\n", sysgetpid());

        // Sleep for 3 seconds
        syssleep(sysgetpid(), seconds);

    }
}
