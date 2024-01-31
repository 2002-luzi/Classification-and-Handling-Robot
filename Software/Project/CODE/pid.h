#ifndef _PID_H
#define _PID_H

#include <headfile.h>

typedef struct  PID
{
    float p;
    float i;
    float d;
    float sum_i;
    float out;
    float last_e;
    float dt;
}pid;

void pid_init(void);
void pid_cal(float tar, float cur, pid* m);

extern pid pid1, pid2, pid3, pid4, pid_yaw;
#endif