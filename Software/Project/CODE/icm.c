#include "icm.h"

void icm_read_timer3(void *parameter);
float32_t fhan(float32_t x1, float32_t x2, float32_t r0, float32_t h0);
void adrc_filter(adrc *f);

float32_t theta = 0;
float32_t bias = 0;
float32_t realGryo_x, realGryo_y, realGryo_z, realAcc_x, realAcc_y, realAcc_z;
float32_t k1 = 0.3, k2 = 0.6, last_gryo_z = 0, last_acc_x = 0;
float32_t last_acc_x1 = 0, lpf_acc_x = 0;
float32_t speed_x, speed_y = 0; // speed_x is got from encoder value, speedX is got from acc value
float32_t x_position = 0, y_position = 0;

adrc adrc_acc_x, cur, nxt;

void ICM_INIT(void)
{
    icm20602_init_spi();
    rt_timer_t timer3;
    timer3 = rt_timer_create("timer3", icm_read_timer3, RT_NULL, 10, RT_TIMER_FLAG_PERIODIC);
    rt_thread_mdelay(2000);

    for (int i = 0; i < 200; i++)
    {
        get_icm20602_gyro_spi();
        realGryo_z = (float)icm_gyro_z / 16.4;
        bias += realGryo_z;
        rt_thread_mdelay(5);
    }
    bias = bias / 200;

    if (timer3 != RT_NULL)
    {
        rt_timer_start(timer3);
    }

    // adrc parameters initialize
    adrc_acc_x.r = 30000;
    adrc_acc_x.h0 = 0.02;
    adrc_acc_x.last = 0;
    adrc_acc_x.last1 = 0;
}

void icm_read_timer3(void *parameter)
{
    get_icm20602_accdata_spi();
    get_icm20602_gyro_spi();

    realGryo_x = (float)icm_gyro_x / 16.4;
    realGryo_y = (float)icm_gyro_y / 16.4;
    realGryo_z = (float)icm_gyro_z / 16.4;
    realGryo_z -= bias;
    realGryo_z *= (-1.0);
    last_gryo_z = last_gryo_z * (1 - k2) + k2 * realGryo_z;
    // last_gryo_z = realGryo_z - last_gryo_z;
    // last_gryo_z -= 0.02385;

    theta += (last_gryo_z * 0.01);
    // realGryo_z -= 0.032;
    if (theta < 0)
        theta += 360;
    else if (theta > 360)
        theta -= 360;

    static int cnt = 0;
    if(motion_flag == 3)
    {
        cnt++;
        if(cnt > 150)
            motion_flag = 1, cnt = 0;
    } 
    
    if (car.uart == SP_OK)
    {
        // data_analysis();
        car.uart = SP_WAIT;
    }
    //uart_putfloat(USART_1, realGryo_z), rt_kprintf(","), uart_putfloat(USART_1, last_gryo_z), rt_kprintf("\n");
}

float32_t sgn(float32_t x)
{
    if (fabs(x) < 1e-7)
        return 0;
    return x < 0 ? -1 : 1;
}

float32_t fsg(float32_t x, float32_t d) {
	return (sgn(x + d) - sgn(x - d)) / 2;
}

float32_t fhan(float32_t x1, float32_t x2, float32_t r0, float32_t h0) { // FST_calculating
	float32_t d, y, a0, a1, a2, a, sy, sa;
	d = r0 * h0 * h0, a0 = x2 * h0;
	y = x1 + a0;
	a1 = sqrt(d * (d + 8 * fabs(y)));
	a2 = a0 + sgn(y) * (a1 - d) * .5;
	a = (a0 + y) * fsg(y, d) + a2 * (1 - fsg(y, d));
	sa = (sgn(a + d) - sgn(a - d)) / 2;
	return - r0 * (a / d) * fsg(y, d) - r0 * sgn(a) * (1 - fsg(a, d));
}

float32_t fhan1(float32_t x1, float32_t x2, float32_t r0, float32_t h0)
{ // FST_calculating
    float32_t d, y, a0, a1, a2, a, sy, sa, h0Square;
    // d = r0 * h0 * h0, a0 = x2 * h0;
    arm_mult_f32(&h0, &h0, &h0Square, 1);
    arm_mult_f32(&r0, &h0Square, &d, 1);
    arm_mult_f32(&x2, &h0, &a0, 1);
    // y = x1 + a0;
    arm_add_f32(&x1, &a0, &y, 1);
    // a1 = sqrt(d * (d + 8 * fabs(y)));
    float32_t temp;
    temp = d * (d + 8 * fabs(y));
    arm_sqrt_f32(temp, &a1);
    a2 = a0 + sgn(y) * (a1 - d) * .5;
    a = (a0 + y - a2) * sy + a2;
    sa = (sgn(a + d) - sgn(a - d)) / 2;
    return -r0 * (a / d - sgn(a)) * sa - r0 * sgn(a);
}

void adrc_filter(adrc *f)
{
    f->last = f->last + f->h0 * f->last1;
    f->last1 = f->last1 + f->h0 * f->fh;
}