#ifndef _PORTS_H
#define _PORTS_H

#include "types.h"

uchar_8 port_byte_in (ushort_16 port);
void port_byte_out (ushort_16 port, uchar_8 data);
#endif