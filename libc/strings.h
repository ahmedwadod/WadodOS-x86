#ifndef _STRINGS_H
#define _STRINGS_H

#include "../cpu/types.h"
#include "stdlib.h"

void int_to_ascii(int n, char str[]);
void reverse(char s[]);
int strlen(char s[]);
void droplastchar(char* str);
void appendchar(char* str, char letter);
void combine(char* origin, char* addition);
bool strcmp_len(char *a, char *b, int count);

#endif