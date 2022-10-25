// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <meminfo.h>

void printMemInfo() {
    
    // Get memory info
    TMemInfo memInfo;
    sysmeminfo(&memInfo);

    // Print memory memory info
    printf("Total memory: %d bytes (%d MB)\n", memInfo.total, memInfo.total / 1024 / 1024);
    printf("Used memory: %d bytes (%d MB)\n", memInfo.used, memInfo.used / 1024 / 1024);
    printf("Free memory: %d bytes (%d MB)\n", memInfo.free, memInfo.free / 1024 / 1024);

}
