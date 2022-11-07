// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <interrupts/syscalls.h>

// rax -> rdi,
// rdi -> rsi,
// rsi -> rdx,
// rdx -> rcx,
// rcx -> r8,
// r8 ->  r9
typedef int64_t (*TSyscallHandler) (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

int64_t sysread(uint64_t fd, char * buffer, int64_t bytes);
int64_t syswrite(uint64_t fd, const char * buffer, int64_t bytes);

void *  sysmalloc(uint64_t size);
void    sysfree(void * ptr);
void *  sysrealloc(void * ptr, uint64_t size);
void    sysmeminfo(TMemInfo* memInfo);
void *  syscalloc(uint64_t nmemb, uint64_t size);

pid_t syscreateprocess(uint64_t rip, int ground, int priority, int argc, char * argv[]);
void sysexit();

// TODO: Remove this?
void sysexec(pid_t pid);
void sysps();

int64_t syskill(pid_t pid);

int64_t syswaitpid(pid_t pid);

void sysnice(pid_t pid, int priority);

int64_t sysblock(pid_t pid);
int64_t sysunblock(pid_t pid);
int64_t syssleep(pid_t pid, uint64_t seconds);

void sysyield();
int64_t sysgetpid();

sem_t    syssemopen(const char * name, uint64_t value);
int64_t  syssemwait(sem_t sem);
int64_t  syssempost(sem_t sem);
int64_t  syssemclose(sem_t sem);
int64_t  syssemunlink(const char * name);
TSemInfo *sysseminfo(uint64_t *size);

sem_t    sysseminit(uint64_t value);
int64_t  syssemdestroy(sem_t sem);

int64_t sysdup(pid_t pid, fd_t prev, fd_t new);
int64_t syspipe(fd_t fd[2]);



TSyscallHandler syscallHandlers[] = {
    //0x00
    (TSyscallHandler) sysread,
    //0x01
    (TSyscallHandler) syswrite,

    //0x02
    (TSyscallHandler) sysmalloc,
    //0x03
    (TSyscallHandler) sysfree,
    //0x04
    (TSyscallHandler) sysrealloc,
    //0x05
    (TSyscallHandler) syscalloc,
    //0x06
    (TSyscallHandler) sysmeminfo,

    //0x07
    (TSyscallHandler) syscreateprocess,
    //0x08
    (TSyscallHandler) sysexit,
    //0x09
    (TSyscallHandler) sysexec,
    //0x0A
    (TSyscallHandler) sysps,
    //0x0B
    (TSyscallHandler) syskill,
    //0x0C
    (TSyscallHandler) syswaitpid,
    //0x0D
    (TSyscallHandler) sysnice,
    //0x0E
    (TSyscallHandler) sysblock,
    //0x0F
    (TSyscallHandler) sysunblock,
    //0x10
    (TSyscallHandler) syssleep,
    //0x11
    (TSyscallHandler) sysyield,
    //0x12
    (TSyscallHandler) sysgetpid,

    //0x13
    (TSyscallHandler) syssemopen,
    //0x14
    (TSyscallHandler) syssemwait,
    //0x15
    (TSyscallHandler) syssempost,
    //0x16
    (TSyscallHandler) syssemclose,
    //0x17
    (TSyscallHandler) syssemunlink,
    //0x18
    (TSyscallHandler) sysseminfo,
    //0x19
    (TSyscallHandler) sysseminit,
    //0x1A
    (TSyscallHandler) syssemdestroy,

    //0x1B
    (TSyscallHandler) sysdup,
    //0x1C
    (TSyscallHandler) syspipe,


};

static uint64_t syscallHandlersDim = sizeof(syscallHandlers) / sizeof(syscallHandlers[0]);

int64_t syscallDispatcher(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8) {
    if(rax >= syscallHandlersDim)
        return -1;

    // function call
    return syscallHandlers[rax](rdi, rsi, rdx, rcx, r8);
}

int64_t syswrite(uint64_t fd, const char * buffer, int64_t bytes) {
    if(fd != STDOUT && fd != STDERR)
        return -1;

    if(fd == STDOUT){
        PCBType * process = getActiveProcess();
        fd = process->fd[STDOUT];
    }

    if (fd>=3){
        return pipeWrite(fd, buffer, bytes);
    }

    int64_t bytesWritten;
    static gcolor RED   = {0xFF, 0x00, 0x00};
    gcolor foreground = (fd == STDOUT)? gGetDefaultForeground() : RED;

    for(bytesWritten = 0; bytesWritten < bytes; bytesWritten++) {
        gPutcharColor(buffer[bytesWritten], gGetDefaultBackground(), foreground);
    }
    return bytesWritten;
}

int64_t sysread(uint64_t fd, char * buffer, int64_t bytes) {
    if(fd != STDIN) {
       return -1;
    }

    
    PCBType * process = getActiveProcess();
    fd = process->fd[STDIN];

    if (fd == STDIN && getActiveProcess()->ground == 1) {
        // I dont let you because you are in background
        return -1;
    }

    if(fd >= 3){
        return pipeRead(fd, buffer, bytes);
    }

    //read normally
    int64_t i = 0;
    char c;

    // Block if there is no data to read
    while (bufferIsEmpty()) {
        blockProcessOnInput(getActivePid());
    }

    for(;i < bytes && (c = getchar()) != 0; i++) {
        buffer[i] = c;
    }
    return i;
}


// --------- Memory manager ---------------------------------------------------------------
void * sysmalloc(uint64_t size) {
    return malloc(size);
}

void sysfree(void * ptr) {
    free(ptr);
}

void * sysrealloc(void * ptr, uint64_t size) {
    return realloc(ptr, size);
}

void * syscalloc(uint64_t nmemb, uint64_t size) {
    return calloc(nmemb, size);
}

void sysmeminfo(TMemInfo* memInfo) {
    if(memInfo == NULL) return; //TODO: Clearer return
    meminfo(memInfo);
}

// ------------ Process Manager ----------------
pid_t syscreateprocess(uint64_t rip, int ground, int priority, int argc, char * argv[]){
    //TODO: argv == NULL?
    pid_t pid =  newProcess(rip, ground, priority, argc, argv);
   // exec(pid);
    return pid;
}

void sysexit(){
    killProcess(getActivePid());
    yield();
}

void sysexec(pid_t pid){
    exec(pid);
    yield();
}

void sysps(){
    printAllProcess();
}

int64_t syskill(pid_t pid){
    return killProcess(pid);
}

int64_t syswaitpid(pid_t pid){
    return waitProcess(pid);
}

void sysnice(pid_t pid, int priority){
    changePriority(pid, priority);
}

int64_t sysblock(pid_t pid){
    return blockProcess(pid);
}

int64_t sysunblock(pid_t pid){
    return unblockProcess(pid);
}

int64_t syssleep(pid_t pid, uint64_t seconds){
    return sleepProcess(pid, seconds);
}

void sysyield(){
    yield();
}

int64_t sysgetpid(){
    return getActivePid();
}

// -------------- Semaphores -------------------
sem_t syssemopen(const char * name, uint64_t value){
    if (name == NULL) {
        return -1;
    }

    return sem_open(name, value);
}

int64_t syssemwait(sem_t sem){
    return sem_wait(sem);
}

int64_t syssempost(sem_t sem){
    return sem_post(sem);
}

int64_t syssemclose(sem_t sem){
    return sem_close(sem);
}

int64_t syssemunlink(const char * name){
    if (name == NULL) {
        return -1;
    }

    return sem_unlink(name);
}

TSemInfo *sysseminfo(uint64_t *size){
    if (size == NULL) {
        return NULL;
    }

    return sem_info(size);
}

sem_t sysseminit(uint64_t value){
    return sem_init(value);
}

int64_t syssemdestroy(sem_t sem){
    return sem_destroy(sem);
}

// ---------------- Pipes --------------------

int64_t sysdup(pid_t pid, fd_t prev, fd_t new){
    return dup(pid, prev, new);
}

int64_t syspipe(fd_t fd[2]){
    return pipe(fd);
}
