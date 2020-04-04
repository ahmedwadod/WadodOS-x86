#include "stdlib.h"

/**
 * Copy number(count) of bytes from one pointer(source) to another(dest).
 * 
 * No return.
 */
void memory_copy(void *source, void *dest, int count) {
    int i;
    for (i = 0; i < count; i++) {
        *(dest + i) = *(source + i);
    }
    *(dest + count) = '\0';
}

/**
 * Repeat the same byte(value) in memory starting from pointer(dest)
 * for serveral times(count).
 * 
 * No return.
*/
void memory_set(void *dest, char value, int count) {
    char *temp = (char *)dest;
    for ( ; count != 0; count--) *temp++ = value;
}

/**
 * Allocates space in memory at offset of (ptr) by number of bytes according to (size).
 * 
 * No return.
*/
void kmalloc(void *ptr, int size)
{
    memory_set(ptr, 0, size);
}

/** 
 * Setup IDT, PIC, ISR, IRQ
*/
void setup_cpu()
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
    print_error(errormsg, errorno);
    __asm__ volatile("int $19");

    while(1);
}