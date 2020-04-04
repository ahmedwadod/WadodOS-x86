#ifndef _STDLIB_H
#define _STDLIB_H

#include "../cpu/types.h"
#include "../cpu/timer.h"
#include "../cpu/isr.h"
#include "stdio.h"

void setup_cpu();
void memory_copy(void *source, void *dest, int count);
void memory_set(void *dest, char val, int len);
void kmalloc(void *ptr, int size);
void sleep(int ms);
void kpanic(char* errormsg, int errorno);

#endif