#ifndef _STRING_H
#define _STRING_H
#define LONG_MIN -1L

#include <stdint.h>
#include <syscalls.h>

// Returns the length of the string
uint64_t strlen(const char * str);

// Compares both strings until the nth character
int64_t strncmp(const char * str1, const char * str2, uint64_t n);

// Compares both strings
int64_t strcmp(const char * str1, const char * str2);

// Returns the numeric value of the number at the start of the str
// If no valid number was found or the base was invalid, it returns -1
// Accepts bases between 2 and 36
// Address of the first character after the end of the number is placed on endptr
// Ignores spaces
int64_t strtol(const char *str, const char **endptr, int base);

// If a divider is found, it places the left part into leftBuff and the right part into rightBuff
// It ignores spaces, only divides upon the first appearance of the divider
// Returns 0 if no divider was found, leftBuff equals str
// Returns 1 if a divider was found
int64_t strDivide(const char * str, char * leftBuff, char * rightBuff, char divider);

// Remove spaces at the beginning and at the end of the str
void trim(char *str);

// Memset 
void * memset(void * ptr, int32_t c, uint64_t length);

//https://opensource.apple.com/source/BerkeleyDB/BerkeleyDB-21/db/clib/strchr.c.auto.html
char *strchr(const char *p, int ch);

// Receives a number and returns an allocated string with the number in base 10
char * itoa(int64_t num);

// Reverse a string
void reverse(char *str, int len);

#endif//_STRING_H
