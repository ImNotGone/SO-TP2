// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <syscalls.h>
#include <_stdio.h>
#include <_string.h>

#define INITIAL_PHYLOS 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0

int *state;

sem_t forkMutex;

uint64_t philosopherCount = INITIAL_PHYLOS;

sem_t *semaphores;
pid_t *philosophers;

int getLeftIndex(int id) { return (id + philosopherCount - 1) % philosopherCount; }

int getRightIndex(int id) { return (id + 1) % philosopherCount; }

void printState(void) {
  for (int i = 0; i < philosopherCount; i++) {
    printf(" %c ", state[i] == EATING ? 'E' : '.');
  }
  putchar('\n');
}

void test(int id) {
    if (state[id] == HUNGRY && state[getLeftIndex(id)] != EATING &&
        state[getRightIndex(id)] != EATING) {
        // state that eating
        state[id] = EATING;


        printState();


        // sem_post(&S[phnum]) has no effect
        // during takefork
        // used to wake up hungry philosophers
        // during putfork
        syssempost(semaphores[id]);
    }
}

// take up chopsticks
void take_fork(int id) {

    syssemwait(forkMutex);

    // state that hungry
    state[id] = HUNGRY;

    // eat if neighbours are not eating
    test(id);

    syssempost(forkMutex);

    // if unable to eat wait to be signalled
    syssemwait(semaphores[id]);
}

// put down chopsticks
void put_fork(int id) {

    syssemwait(forkMutex);

    // state that thinking
    state[id] = THINKING;


    test(getLeftIndex(id));
    test(getRightIndex(id));

    syssempost(forkMutex);
}

void *philosopher(int argc, char *argv[]) {

    const char *endptr;
    int philoId = strtol(argv[1], &endptr, 10);


    while (1) {

        syssleep(sysgetpid(), 1);

        take_fork(philoId);

        // Get a pseudo random number
        int seconds = sysgetpid() % 3 + 1;

        syssleep(sysgetpid(), seconds);

        put_fork(philoId);
    }
}

void createPhilosophers() {

    printf("Creating %d philosophers\n", philosopherCount);

    // initialize the semaphores
    forkMutex = sysseminit(1);

    for (int i = 0; i < philosopherCount; i++) {

        char *arg[] = {"philosopher", itoa(i)};

        // create philosopher processes
        philosophers[i] = syscreateprocess((uint64_t) philosopher, 1, 1, 2, arg);

        semaphores[i] = sysseminit(0);

        sysunblock(philosophers[i]);

        sysfree(arg[1]);
    }

}

void killPhilosophers() {
    for (int i = 0; i < philosopherCount; i++) {
        syskill(philosophers[i]);
        syssemdestroy(semaphores[i]);
    }
    syssemdestroy(forkMutex);
}

void phylo(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(STDERR, "Usage: phylo <max_phylosophers>\n");
        return;
    }

    const char * aux;
    int maxPhylos = strtol(argv[1], &aux, 10);
    if (maxPhylos <= 0 || *aux != '\0') {
        fprintf(STDERR, "Invalid number of phylosophers\n");
        return;
    }

    semaphores = sysmalloc(sizeof(sem_t) * maxPhylos);
    philosophers = sysmalloc(sizeof(pid_t) * maxPhylos);


    while (1) {

        createPhilosophers();

        int c;
        while ((c = getchar()) != 'q' && c != 'a' && c != 'r') {

            if (c == EOF) {

                // If we are in background, don't kill the philosophers
                for (int i = 0; i < philosopherCount; i++) {
                    syswaitpid(philosophers[i]);
                }

                sysfree(semaphores);
                sysfree(philosophers);

                return;
            }

            fprintf(STDERR, "Invalid command. Use 'q' to quit, 'a' to add a philosopher or 'r' to remove one philosopher\n");
        }


        if (c == 'q') {
            break;
        }

        killPhilosophers();

        if (c == 'a') {
            if (philosopherCount < maxPhylos) {
                philosopherCount++;
            }else{
                fprintf(STDERR, "Max philosophers reached\n");
            }
        }

        if (c == 'r') {
            if (philosopherCount > 5) {
                philosopherCount--;
            }else{
                fprintf(STDERR, "You can't remove more philosophers\n");
            }
        }

    }

    killPhilosophers();

    sysfree(semaphores);
    sysfree(philosophers);

}


