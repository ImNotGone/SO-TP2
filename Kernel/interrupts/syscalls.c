// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <interrupts/syscalls.h>

int8_t regSaved;
int64_t registerSnapshot[17];

void saveRegs(const int64_t *registers) {
    for (int i = 0; i < 17; i++)
        registerSnapshot[i] = registers[i];
    regSaved = 1;
}

// rax -> rdi,
// rdi -> rsi,
// rsi -> rdx,
// rdx -> rcx,
// rcx -> r8,
// r8 ->  r9
typedef int64_t (*TSyscallHandler) (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

int64_t sysread(uint64_t fd, char * buffer, int64_t bytes);
int64_t syswrite(uint64_t fd, const char * buffer, int64_t bytes);

void    systime(TTime * ts);

int64_t sysmemdump(uint64_t address, int8_t *memData);
int64_t sysregdump(TRegs *regs);

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

int64_t sysdup(pid_t pid, uint64_t prev, uint64_t new);



TSyscallHandler syscallHandlers[] = {
    //0x00
    (TSyscallHandler) sysread,
    //0x01
    (TSyscallHandler) syswrite,

    //0x02
    (TSyscallHandler) systime,

    //0x03
    (TSyscallHandler) sysmemdump,
    //0x04
    (TSyscallHandler) sysregdump,

    //0x05
    (TSyscallHandler) sysmalloc,
    //0x06
    (TSyscallHandler) sysfree,
    //0x07
    (TSyscallHandler) sysrealloc,
    //0x08
    (TSyscallHandler) syscalloc,
    //0x09
    (TSyscallHandler) sysmeminfo,

    //0x0A
    (TSyscallHandler) syscreateprocess,
    //0x0B
    (TSyscallHandler) sysexit,
    //0x0C
    (TSyscallHandler) sysexec,
    //0x0D
    (TSyscallHandler) sysps,
    //0x0E
    (TSyscallHandler) syskill,
    //0x0F
    (TSyscallHandler) syswaitpid,
    //0x10
    (TSyscallHandler) sysnice,
    //0x11
    (TSyscallHandler) sysblock,
    //0x12
    (TSyscallHandler) sysunblock,
    //0x13
    (TSyscallHandler) syssleep,
    //0x14
    (TSyscallHandler) sysyield,
    //0x15
    (TSyscallHandler) sysgetpid,

    //0x16
    (TSyscallHandler) syssemopen,
    //0x17
    (TSyscallHandler) syssemwait,
    //0x18
    (TSyscallHandler) syssempost,
    //0x19
    (TSyscallHandler) syssemclose,
    //0x1A
    (TSyscallHandler) syssemunlink,
    //0x1B
    (TSyscallHandler) sysseminfo,
    //0x1C
    (TSyscallHandler) sysseminit,
    //0x1D
    (TSyscallHandler) syssemdestroy,

    //0x1E
    (TSyscallHandler) sysdup,


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
        //return pipewrite
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
    if(fd != STDIN)
        return -1;

    PCBType * process = getActiveProcess();
    fd = process->fd[STDIN];

    if(fd >= 3){
        //return piperead
    }

    //read normally
    int64_t i = 0;
    char c;
    for(;i < bytes && (c = getchar()) != 0; i++) {
        buffer[i] = c;
    }
    return i;
}

// Escribe la info recibida del rtc en la estructura t
void systime(TTime * t) {
    if(t == NULL) return; //TODO: clearer return
    t->year = getRTCYear();
    t->month = getRTCMonth();
    t->day = getRTCDayOfMonth();
    t->hour = getRTCHours();
    t->min = getRTCMinutes();
    t->sec = getRTCSeconds();
    return;
}

// Escribe los MEM_DUMP_SIZE bytes desde la direccion de memoria indicada en memData
int64_t sysmemdump(uint64_t address, int8_t *memData) {
    // No pudimos resolver el acceso a una direccion de memoria mayor a MAX_MEM_ADDRESS
    if (address > MAX_MEM_ADDRESS || memData == NULL)
        return -1;
    int8_t *memDir = (int8_t *) address;
    int i;
    for (i = 0; i < MEM_DUMP_SIZE; i++) {
        if (address + i > MAX_MEM_ADDRESS)
            return i;
        memData[i] = *(memDir + i);
    }
    return i;
}

// Si se guardaron registros los escribe en regs y retorna 1
// Si no se habian guardado ningun registro retorna 0 y no escribe nada
int64_t sysregdump(TRegs *regs) {
    if (!regSaved || regs == NULL)
        return 0;
    regs->rax = registerSnapshot[0];
    regs->rbx = registerSnapshot[1];
    regs->rcx = registerSnapshot[2];
    regs->rdx = registerSnapshot[3];
    regs->rsi = registerSnapshot[4];
    regs->rdi = registerSnapshot[5];
    regs->rbp = registerSnapshot[6];
    regs->rsp = registerSnapshot[7];
    regs->r8  = registerSnapshot[8];
    regs->r9  = registerSnapshot[9];
    regs->r10 = registerSnapshot[10];
    regs->r11 = registerSnapshot[11];
    regs->r12 = registerSnapshot[12];
    regs->r13 = registerSnapshot[13];
    regs->r14 = registerSnapshot[14];
    regs->r15 = registerSnapshot[15];
    regs->rip = registerSnapshot[16];
    return 1;
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
};

int64_t sysunblock(pid_t pid){
    return unblockProcess(pid);
};

int64_t syssleep(pid_t pid, uint64_t seconds){
    return sleepProcess(pid, seconds);
};

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

int64_t sysdup(pid_t pid, uint64_t prev, uint64_t new){
    return dup(pid, prev, new);
}
