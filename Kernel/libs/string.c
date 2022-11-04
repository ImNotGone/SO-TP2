#include <libs/string.h>

int strcmp(const char* s1, const char* s2) {
    int i;
    for (i = 0; s1[i] && s1[i] == s2[i]; i++);
    return s1[i] - s2[i];
}

uint64_t strlen(char *string){
    int i = 0;
    while(string[i++]);
    return i - 1;
}

void strcpy(char *string, char *target){
    while(*target != 0){
        *(string++) = *(target++);
    }
    *string = 0;
}
