#include "../include/fs.h"

void init_fs(device_t* dev)
{

}

void datetime_to_fatdatetime(datetime_t* dt, ushort_16* fattime, ushort_16* fatdate)
{
    ushort_16 hour, minute, second = 0;
    hour = dt->hour & 0x0F;
    hour += (dt->hour / 16) * 10;
    hour &= 0b00011111; // 5 bits
    minute = dt->minute & 0x0F;
    minute += (dt->minute / 16) * 10;
    minute &= 0b00111111; // 6 bits
    second = dt->second & 0x0F;
    second += (dt->second / 16) * 10;
    second &= 0b00011111; // 5 bits
    *fattime = (hour << 11) | ((minute) << 5) | second;

    ushort_16 year, month, day = 0;
    year = dt->year & 0x0F;
    year += (dt->year / 16) * 10;
    year &= 0b01111111; // 7 bits
    month = dt->month & 0x0F;
    month &= 0b00001111; // 4 bits
    day = dt->day & 0x0F;
    day += (dt->day / 16) * 10;
    day &= 0b00011111; // 5 bits
    *fatdate = (year << 9) | (month << 5) | day;
}

void fatdatetime_todatetime(ushort_16* fattime, ushort_16* fatdate, datetime_t* dt)
{
    ushort_16 hour, minute, second;
    hour = (*fattime  >> 11);
    dt->hour = 0;
    dt->hour += hour % 10;
    dt->hour += (hour/10) * 16;
    minute = (*fattime & 0b0000011111100000) >> 5;
    dt->minute = 0;
    dt->minute += minute % 10;
    dt->minute += (minute/10) * 16;
    second = *fattime & 0b0000000000011111;
    dt->second = 0;
    dt->second += second % 10;
    dt->second += (second/10) * 16;

    ushort_16 year, month, day;
    year = (*fatdate >> 9);
    dt->year = 0;
    dt->year += year % 10;
    dt->year += (year/10) * 16;
    month = (*fatdate & 0b0000000111100000) >> 5;
    dt->month = 0;
    dt->month += month % 10;
    dt->month += (month/10) * 16;
    day = (*fatdate & 0b0000000000011111);
    dt->day = 0;
    dt->day += day % 10;
    dt->day += (day/10) * 16;
}