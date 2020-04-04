#include "timer.h"

/** 
 * Trigger a callback every time clock with requecny freq. freq in Hz.
 * Min freq = 18Hz, Max freq = 65535Hz
*/
void timer_start_listening(uint_32 freq, void* timerCallback)
{
    register_interrupt_handler(IRQ0, timerCallback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    uint_32 divisor = 1193180 / freq;
    uchar_8 low  = (uchar_8)(divisor & 0x000000FF);
    uchar_8 high = (uchar_8)( (divisor >> 8) & 0x000000FF);
    /* Send the command */
    port_byte_out(0x43, 0x36); /* Command port */
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}

void timer_stop_listening()
{
    unregister_interrupt_handler(IRQ0);
}