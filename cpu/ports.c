#include "ports.h"

/**
 * Read a byte from the specified port
 */
uchar_8 port_byte_in (ushort_16 port) {
    uchar_8 result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

/**
 * Writes a byte to a specific port
*/
void port_byte_out (ushort_16 port, uchar_8 data) {

    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
    return;
}
