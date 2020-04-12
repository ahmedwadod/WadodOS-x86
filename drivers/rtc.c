#include "../include/rtc.h"
#include "../include/ports.h"

uchar_8 rtc_get_year()
{
	port_byte_out(0x70, 0x09);
	return port_byte_in(0x71);
}

uchar_8 rtc_get_month()
{
	port_byte_out(0x70, 0x08);
	return port_byte_in(0x71);
}

uchar_8 rtc_get_day()
{
	port_byte_out(0x70, 0x07);
	return port_byte_in(0x71);
}

uchar_8 rtc_get_weekday()
{
	port_byte_out(0x70, 0x06);
	return port_byte_in(0x71);
}

uchar_8 rtc_get_hour()
{
	port_byte_out(0x70, 0x04);
	return port_byte_in(0x71);
}

uchar_8 rtc_get_minute()
{
	port_byte_out(0x70, 0x02);
	return port_byte_in(0x71);
}

uchar_8 rtc_get_second()
{
	port_byte_out(0x70, 0x00);
	return port_byte_in(0x71);
}