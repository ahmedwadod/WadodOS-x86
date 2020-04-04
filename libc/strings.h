#ifndef _STRINGS_H
#define _STRINGS_H

#include "../cpu/types.h"

void int_to_ascii(int n, char str[]);
void reverse(char s[]);
int strlen(char s[]);
void droplastchar(char* str);
void appendchar(char* str, char letter);
char strcmp(char *a, char *b);
char strcmp_len(char *a, char *b, int count);

#endif