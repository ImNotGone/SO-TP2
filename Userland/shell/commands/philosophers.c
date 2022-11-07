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
    int philoId = strtol(argv[0], &endptr, 10);


    while (1) {
        
        syssleep(sysgetpid(), 1);

        take_fork(philoId);

        // Get a pseudo random number
        int seconds = sysgetpid() % 3 + 1;

        syssleep(sysgetpid(), 1);

        put_fork(philoId);
    }
}

void createPhilosophers() {

    printf("Creating %d philosophers\n", philosopherCount);
   
    // initialize the semaphores
    forkMutex = sysseminit(1);

    for (int i = 0; i < philosopherCount; i++) {

        char *arg[] = {itoa(i)};

        // create philosopher processes
        philosophers[i] = syscreateprocess((uint64_t) philosopher, 1, 1, 1, arg);

        semaphores[i] = sysseminit(0);

        sysunblock(philosophers[i]);

        sysfree(arg[0]);
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

        char c;
        while ((c = getchar()) != 'q' && c != 'a' && c != 'r') {
            fprintf(STDERR, "Invalid command. Use 'q' to quit, 'a' to add a philosopher or 'd' to remove one philosopher\n");
        }
                    

        if (c == 'q') {
            break;
        }

        killPhilosophers();

        if (c == 'a') {
            if (philosopherCount < maxPhylos) {
                philosopherCount++;
            }
        }

        if (c == 'r') {
            if (philosopherCount > 0) {
                philosopherCount--;
            }
        }

    }

    killPhilosophers();

    sysfree(semaphores);
    sysfree(philosophers);

}


