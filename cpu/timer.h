#ifndef _TIMER_H
#define _TIMER_H

#include "types.h"
#include "isr.h"
#include "../drivers/screen.h"

void timer_start_listening(uint_32 freq, void* timerCallback);
void timer_stop_listening();

#endif