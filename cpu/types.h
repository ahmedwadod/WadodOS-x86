#ifndef _TYPES_H
#define _TYPES_H

typedef unsigned int   uint_32;
typedef unsigned short ushort_16;
typedef unsigned char  uchar_8;

typedef unsigned char bool;
#define true 1
#define false 0

#define low_16(address) (ushort_16)((address) & 0xFFFF)
#define high_16(address) (ushort_16)(((address) >> 16) & 0xFFFF)

#endif
