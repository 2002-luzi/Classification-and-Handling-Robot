#ifndef _encoder_h
#define _encoder_h

#include <headfile.h>

extern int16 speed_1, speed_2, speed_3, speed_4;

void encoder_init(void);
void encoder_get(void);
#endif