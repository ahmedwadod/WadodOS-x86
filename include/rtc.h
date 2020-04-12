#ifndef _RTC_H
#define _RTC_H

#include "stdint.h"

uchar_8 rtc_get_year();
uchar_8 rtc_get_month();
uchar_8 rtc_get_day();
uchar_8 rtc_get_weekday();
uchar_8 rtc_get_hour();
uchar_8 rtc_get_minute();
uchar_8 rtc_get_second();

#endif