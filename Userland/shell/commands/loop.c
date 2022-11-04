#include <loop.h>

void loop() {
    while (1) {
        
        // Print a greeting

        printf("Hello, world!, I'm a process with PID %d\n", sysgetpid());

        // Sleep for 3 seconds
        syssleep(sysgetpid(), 3);

    }
}
