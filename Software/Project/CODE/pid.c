#include "pid.h"

pid pid1, pid2, pid3, pid4, pid_yaw;

void pid_init(void)
{
    pid1.p = pid2.p = pid3.p = pid4.p = 6;
    pid1.i = pid2.i = pid3.i = pid4.i =	8;
    pid1.d = pid2.d = pid3.d = pid4.d =0;
    pid1.last_e = pid2.last_e = pid3.last_e = pid4.last_e =0;
    pid1.dt = pid2.dt = pid3.dt = pid4.dt =0;
    pid1.sum_i = pid2.sum_i = pid3.sum_i = pid4.sum_i = 0;
    pid1.out = pid2.out = pid3.out = pid4.out =0;
	
	/*	pid2.p = 6;
    pid2.i = 0.8;
    pid2.d = 0;
    pid2.last_e = 0;
    pid2.dt = 0;
    pid2.sum_i = 0;
    pid2.out = 0;

    pid3.p = 6;
    pid3.i = 0.8;
    pid3.d = 0;
    pid3.last_e = 0;
    pid3.dt = 0;
    pid3.sum_i = 0;
    pid3.out = 0;*/

    pid1.p = 8;
    pid1.i = 16;
    pid1.d = 0;
    pid1.last_e = 0;
    pid1.dt = 0;
    pid1.sum_i = 0;
    pid1.out = 0;

    pid4.p = 8;
    pid4.i = 16;
    pid4.d = 0;
    pid4.last_e = 0;
    pid4.dt = 0;
    pid4.sum_i = 0;
    pid4.out = 0;
    
    pid_yaw.p = 1;
    pid_yaw.i = 0;
    pid_yaw.d = 1;
    pid_yaw.last_e = 0;
    pid_yaw.dt = 0;
    pid_yaw.sum_i = 0;
    pid_yaw.out = 0;
    return;
}

void pid_cal(float tar, float cur, pid* m)
{
    float e = tar - cur;
    m->sum_i += e;
    m->dt = e - m->last_e;

    if(tar > 0)
    {
        if((cur - tar) - 0.1 * tar > 0)
            m->sum_i = m->sum_i * 0.8;
    }

    if(tar < 0)
    {
        if((tar - cur) + 0.1 * tar > 0)
            m->sum_i = m->sum_i * 0.8;
    }
		
		//if(fabs(tar) < 1e-5)
		//{
				//if(cur >= 10 || cur <= -10)
						//m->sum_i = m->sum_i * 0.90;
		//}
    m->out = m->p * e + m->i * m->sum_i + m->d * m->dt;
    m->last_e = e;
    if(m->out > 10000)
        m->out = 9999;
    if(m->out < -10000)
        m->out = -9999;
}

