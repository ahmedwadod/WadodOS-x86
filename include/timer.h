#ifndef _TIMER_H
#define _TIMER_H

#include "stdint.h"

void timer_start_listening(uint_32 freq, void* timerCallback);
void timer_stop_listening();

#endif