#include "../include/stdtime.h"
#include "../include/rtc.h"
#include "../include/stdio.h"

datetime_t datetime_now()
{
    datetime_t dt;
    dt.year = rtc_get_year();
    dt.month = rtc_get_month();
    dt.day = rtc_get_day();
    dt.weekday = rtc_get_weekday();
    dt.hour = rtc_get_hour();
    dt.minute = rtc_get_minute();
    dt.second = rtc_get_second();

    return dt;
}

void print_datetime(datetime_t dt)
{
    printf("20%x/%x/%x %x:%x:%x", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
}