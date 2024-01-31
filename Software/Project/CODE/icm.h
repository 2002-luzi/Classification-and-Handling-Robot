#ifndef _ICM_H
#define _ICM_H

#include <headfile.h>


typedef struct
{
    float32_t r;
    float32_t h0;
    float32_t last;     //output
    float32_t last1;
    float32_t fh;
}adrc;

void ICM_INIT(void);

extern float32_t theta, realGryo_z, speed_x, speed_y, x_position, y_position;

#endif