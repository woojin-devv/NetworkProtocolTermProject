#ifndef L3_2MIN_TIMER_H
#define L3_2MIN_TIMER_H

#include "mbed.h"

bool getInputWithinTime(Serial& pc, char* buffer, int bufferSize, float timeLimitSec);

#endif