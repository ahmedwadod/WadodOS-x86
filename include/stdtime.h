#ifndef _STDTIME_H
#define _STDTIME_H

#include "stdint.h"

typedef struct
{
    uchar_8 year;
    uchar_8 month;
    uchar_8 day;
    uchar_8 weekday;
    uchar_8 hour;
    uchar_8 minute;
    uchar_8 second;
} datetime_t;

datetime_t datetime_now();
void print_datetime(datetime_t dt);

#endif