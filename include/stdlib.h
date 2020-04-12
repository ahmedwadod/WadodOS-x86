#ifndef _STDLIB_H
#define _STDLIB_H

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#include "stdbool.h"

void itoa(int value, char* str, int base);
void init_cpu();
int kmalloc(void *ptr, int size);
void sleep(int ms);
void kpanic(char* errormsg, int errorno);


static inline void * memcpy(const void * src, void * dest, int n)
{
__asm__ ("cld\n\t"
	"rep\n\t"
	"movsb"
	::"c" (n),"S" (src),"D" (dest)
	);
return dest;
}

static inline void * memmove(const void * src, void * dest, int n)
{
if (dest<src)
__asm__("cld\n\t"
	"rep\n\t"
	"movsb"
	::"c" (n),"S" (src),"D" (dest)
	);
else
__asm__("std\n\t"
	"rep\n\t"
	"movsb"
	::"c" (n),"S" (src+n-1),"D" (dest+n-1)
	);
return dest;
}

static inline int memcmp(const void * cs,const void * ct,int count)
{
register int __res ;
__asm__("cld\n\t"
	"repe\n\t"
	"cmpsb\n\t"
	"je 1f\n\t"
	"movl $1,%%eax\n\t"
	"jl 1f\n\t"
	"negl %%eax\n"
	"1:"
	:"=a" (__res):"0" (0),"D" (cs),"S" (ct),"c" (count)
	);
return __res;
}

static inline void * memchr(const void * cs,char c,int count)
{
register void * __res ;
if (!count)
	return NULL;
__asm__("cld\n\t"
	"repne\n\t"
	"scasb\n\t"
	"je 1f\n\t"
	"movl $1,%0\n"
	"1:\tdecl %0"
	:"=D" (__res):"a" (c),"D" (cs),"c" (count)
	);
return __res;
}

static inline void * memset(void * s,char c,int count)
{
__asm__("cld\n\t"
	"rep\n\t"
	"stosb"
	::"a" (c),"D" (s),"c" (count)
	);
return s;
}

#endif