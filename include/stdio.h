#ifndef _STDIO_H
#define _STDIO_H

#include "stdbool.h"

typedef enum
{
    INPUT_MODE_ONE_CHAR = 0,
    INPUT_MODE_ONE_LINE = 1,
    INPUT_MODE_MULTILINE = 2
} InputMode;

void clear_screen();
void init_keyboard();
void putchar(char c);
void puts(char *str);
void printf(char * format, ...);
char getchar(bool handled);
void gets(char* instr);

#endif