#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void keyboard_handler();
void cleanKeyboardBuffer();
uint8_t getKey();
uint8_t getchar();


#endif//KB_H