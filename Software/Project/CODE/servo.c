#include "servo.h"

void servo_init(void)
{
    pwm_init(SERVO_ROTE, 50, 600); // put_right 360		put_left 860	put_front 612 	put_tail 1120			drop_left 612		drop_tail 860 	drop_right 1120
    pwm_init(SERVO_DROP, 50, 635); // up 365    down   635
    pwm_init(SERVO_GET, 50, GET_UP); // down 500		put 970		up 1050
}

void servo_reset(void)
{
	servo_rote_ctrl(600);		//put_right 360		put_left 860	put_front 612 	put_tail 1120			
																//drop_left 612		drop_tail 860 	drop_right 1120
	pwm_duty(SERVO_DROP, 635);			
	pwm_duty(SERVO_GET, GET_UP);			
}

void servo_get(void)
{
    gpio_set(C5, 1);
    servo_rote_ctrl(600); // put_right 360		put_left 860	put_front 612 	put_tail 1120
                          // drop_left 612		drop_tail 860 	drop_right 1120
    pwm_duty(SERVO_DROP, 635);
    int cnt = abs(GET_UP - GET_DOWN) / 10;
    int dir = (GET_DOWN - GET_UP) < 0 ? -1 : 1;
    for (int i = 1; i <= cnt; i++)
    {
        pwm_duty(SERVO_GET, GET_UP + i * 10 * dir);
        rt_thread_mdelay(8);
    }
    pwm_duty(SERVO_GET, GET_DOWN);
    rt_thread_mdelay(100);
}

void servo_put_left(void)
{

    // rt_thread_mdelay(80);
    servo_rote_ctrl(840); // put_right 360		put_left 860	put_front 612 	put_tail 1120
                          // drop_left 612		drop_tail 860 	drop_right 1120
    // rt_thread_mdelay(300);
    pwm_duty(SERVO_DROP, 635);
    int cnt = abs(GET_UP - GET_PUT) / 10;
    int dir = (GET_PUT - GET_UP) < 0 ? -1 : 1;
    for (int i = 1; i <= cnt; ++i)
    {
        pwm_duty(SERVO_GET, GET_UP + dir * i * 10);
        rt_thread_mdelay(15);
    }
    pwm_duty(SERVO_GET, GET_PUT); // down 470		put 880		up 1000
    // rt_thread_mdelay(400);
    gpio_set(C5, 0);
}

void servo_put_right(void)
{

    // rt_thread_mdelay(80);
    servo_rote_ctrl(340); // put_right 360		put_left 860	put_front 612 	put_tail 1120
                          // drop_left 612		drop_tail 860 	drop_right 1120
    // rt_thread_mdelay(300);
    pwm_duty(SERVO_DROP, 635);
    int cnt = abs(GET_UP - GET_PUT) / 10;
    int dir = (GET_PUT - GET_UP) < 0 ? -1 : 1;
    for (int i = 1; i <= cnt; ++i)
    {
        pwm_duty(SERVO_GET, GET_UP + dir * i * 10);
        rt_thread_mdelay(15);
    }
    pwm_duty(SERVO_GET, GET_PUT); 
    // rt_thread_mdelay(400);
    gpio_set(C5, 0);
}

void servo_put_tail(void)
{

    // rt_thread_mdelay(80);
    servo_rote_ctrl(1110); // put_right 360		put_left 860	put_front 612 	put_tail 1120
                           // drop_left 612		drop_tail 860 	drop_right 1120
    // rt_thread_mdelay(300);
    pwm_duty(SERVO_DROP, 635);
    int cnt = abs(GET_UP - GET_PUT) / 10;
    int dir = (GET_PUT - GET_UP) < 0 ? -1 : 1;
    for (int i = 1; i <= cnt; ++i)
    {
        pwm_duty(SERVO_GET, GET_UP + dir * i * 10);
        rt_thread_mdelay(15);
    }
    pwm_duty(SERVO_GET, GET_PUT); 
    gpio_set(C5, 0);
}

void servo_drop_left(void)
{	
	
	pwm_duty(SERVO_GET, GET_UP);
	//rt_thread_mdelay(80);
	servo_rote_ctrl(600);		//put_right 360		put_left 860	put_front 612 	put_tail 1120			
																//drop_left 612		drop_tail 860 	drop_right 1120
	for(int i = 1; i <= 27; i++)
	{
			pwm_duty(SERVO_DROP, 635 - i * 10);
			rt_thread_mdelay(8);
	}
	rt_thread_mdelay(200);
	pwm_duty(SERVO_DROP, 635);
	//rt_thread_mdelay(400);
	gpio_set(C5, 0);
}

void servo_drop_right(void)
{	
	pwm_duty(SERVO_GET, GET_UP);			//down 470		put 900		up 1000
	servo_rote_ctrl(1110);		//put_right 360		put_left 860	put_front 612 	put_tail 1120			
																//drop_left 612		drop_tail 860 	drop_right 1120
	//rt_thread_mdelay(300);
	for(int i = 1; i <= 27; i++)
	{
			pwm_duty(SERVO_DROP, 635 - i * 10);
			rt_thread_mdelay(8);
	}
	rt_thread_mdelay(200);
	pwm_duty(SERVO_DROP, 635);
	//rt_thread_mdelay(400);
	gpio_set(C5, 0);
}

void servo_drop_tail(void)
{	
	pwm_duty(SERVO_GET, GET_UP);			//down 470		put 900		up 1000
	servo_rote_ctrl(850);		//put_right 360		put_left 860	put_front 612 	put_tail 1120			
																//drop_left 612		drop_tail 860 	drop_right 1120
	//rt_thread_mdelay(300);
	for(int i = 1; i <= 27; i++)
	{
			pwm_duty(SERVO_DROP, 635 - i * 10);
			rt_thread_mdelay(8);
	}
	rt_thread_mdelay(300);
	pwm_duty(SERVO_DROP, 635);
	
	gpio_set(C5, 0);
}

void servo_rote_ctrl(int tar)
{
    static int pwm_lst = 600;
    int dir = (tar - pwm_lst) > 0 ? 1 : -1;
    int num = abs(tar - pwm_lst) / 10;
    for (int i = 1; i <= num; i++)
    {
        pwm_duty(SERVO_ROTE, pwm_lst + dir * i * 10);
        rt_thread_mdelay(15);
    }
    pwm_duty(SERVO_ROTE, tar);
    pwm_lst = tar;
}