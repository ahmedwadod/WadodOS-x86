#include "../include/stdlib.h"
#include "../include/screen.h"
/**
 * Allocates space in memory at offset of (ptr) by number of bytes according to (size).
 * 
 * Returns the physical address
*/
extern int kernel_end;
static int space = 8;
int kmalloc(void *ptr, int size)
{
    int addr = kernel_end + space;
    space += size;
    ptr = addr;
    return addr;
}

/** 
 * Setup IDT, PIC, ISR, IRQ
*/
void init_cpu()
{
    isr_install();
}

/** 
 * Hang processing
*/
static int sleep_counter = 0;
static bool hang = false;
void _sleepLoop()
{
    sleep_counter--;
    if(sleep_counter == 0)
        hang = false;
}

/** 
 * Sleep the kernel for some time in milliseconds(ms)
*/
void sleep(int ms)
{
    sleep_counter = ms;
    timer_start_listening(1000, _sleepLoop);
    hang = true;
    while (hang)
    {
        // Do nothing
    }
    timer_stop_listening();
    
}

// Raise a kernel panic with error message
void kpanic(char* errormsg, int errorno)
{
    char *codestr;
    itoa(errorno, codestr, 16);
    kprint_at_with_attr("Error Code: 0x", -1, -1, RED_ON_WHITE);
    kprint_at_with_attr(codestr, -1, -1, RED_ON_WHITE);
    kprint_at_with_attr("\nError Message: ", -1, -1, RED_ON_WHITE);
    kprint_at_with_attr(errormsg, -1, -1, RED_ON_WHITE);
    kprint("\n");
    __asm__ volatile("int $19");

    while(1);
}

/**
 *  Ansi C "itoa" based on Kernighan & Ritchie's "Ansi C"
 *
 */
void strreverse(char* begin, char* end) 
{
  char aux;
  
  while(end>begin)
    aux=*end, *end--=*begin, *begin++=aux;
}
  
void 
itoa(int value, char* str, int base) 
{
  static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  
  char* wstr = str;
  int sign;
  
  
  // Validate base
  if (base < 2 || base > 35) { 
    *wstr='\0'; 
    return; 
  }

  // Take care of sign
  if ((sign=value) < 0) 
    value = -value;

  // Conversion. Number is reversed.
  do {
    *wstr++ = num[value % base]; 
  } while (value /= base);
  
  if(sign<0) 
    *wstr++='-';

  *wstr='\0';
  // Reverse string
  strreverse(str,wstr-1);
}