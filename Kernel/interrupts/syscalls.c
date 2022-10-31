// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <interrupts/syscalls.h>
#include <libs/memoryManager.h>
#include <drivers/graphics.h>
#include <drivers/keyboard.h>
#include <drivers/RTC.h>
#include <libs/processManager.h>
#include <libs/scheduler.h>
#include <interrupts/interrupts.h>

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
// r10 -> r8,
// r8 ->  r9
typedef int64_t (*TSyscallHandler) (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8);

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
uint64_t syscreateprocess(uint64_t rip, int ground, int priority, int argc, char * argv[]);
void sysexit();
void sysexec(uint64_t pid);


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

};

static uint64_t syscallHandlersDim = sizeof(syscallHandlers) / sizeof(syscallHandlers[0]);

int64_t syscallDispatcher(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8) {
    if(rax >= syscallHandlersDim)
        return -1;

    // function call
    return syscallHandlers[rax](rdi, rsi, rdx, r10, r8);
}

int64_t syswrite(uint64_t fd, const char * buffer, int64_t bytes) {
    if(fd != STDOUT && fd != STDERR) return -1;
    int64_t bytesWritten;
    static gcolor RED   = {0xFF, 0x00, 0x00};
    gcolor foreground = (fd == STDOUT)? gGetDefaultForeground():RED;
    for(bytesWritten = 0; bytesWritten < bytes; bytesWritten++) {
        gPutcharColor(buffer[bytesWritten], gGetDefaultBackground(), foreground);
    }
    return bytesWritten;
}

int64_t sysread(uint64_t fd, char * buffer, int64_t bytes) {
    if(fd != STDIN) return -1;
    int64_t i = 0;
    char c;
    for(;i < bytes && (c = getchar()) != 0; i++) {
        buffer[i] = c;
    }
    return i;
}

// Escribe la info recibida del rtc en la estructura t
void systime(TTime * t) {
    t->year = getRTCYear();
    t->month = getRTCMonth();
    t->day = getRTCDayOfMonth();
    t->hour = getRTCHours();
    t->min = getRTCMinutes();
    t->sec = getRTCSeconds();
}

// Escribe los MEM_DUMP_SIZE bytes desde la direccion de memoria indicada en memData
int64_t sysmemdump(uint64_t address, int8_t *memData) {
    // No pudimos resolver el acceso a una direccion de memoria mayor a MAX_MEM_ADDRESS
    if (address > MAX_MEM_ADDRESS)
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
    if (!regSaved)
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
    meminfo(memInfo);
}

// ------------ Process Manager ----------------
uint64_t syscreateprocess(uint64_t rip, int ground, int priority, int argc, char * argv[]){
    return newProcess(rip, ground, priority, argc, argv);
    //switchContext(newProcess(rip, ground, argc, argv));
    //_irq00Handler();
}

void sysexit(){
    killProcess(getActivePid());
    yield();
}

void sysexec(uint64_t pid){
    exec(pid);
    yield();
}
