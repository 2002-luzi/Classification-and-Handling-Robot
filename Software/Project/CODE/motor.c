#include "motor.h"

float32_t target_1 = 0, target_2 = 0, target_3 = 0, target_4 = 0;
float32_t x = 0, y = 0, z = 0;
status car;
float32_t std_speed = 30;
float32_t x_pos1, y_pos1;
float para_y[10] = {1.57, 1.57, 1.57, 1.57, 1.57, 1.55, 1.53, 1.49, 1.49, 1.49};

void speedChange(void *parameter);
void uart_trans(void *parameter);
void servo_ctrl(void *parameter);
void to_pic(void);
void to_pic2(void);
void pic_mag(void);
void drop_it(void);
void go_out(void);
void go_in(void);
void protect_back(void);
void to_bar(void);
void wait_get(void);
void wait_drop(void);
void to_edge(void);

void flag_init(void)
{
    car.motion = 0;
    car.uart = 0;
    car.pic_flag = 0;
    car.send_flag = 0;
    car.twice_flag = 0;
    car.to_pic_cnt = 0;
    car.is_pic = 0;
    car.pic_stop = 0;
    car.send = 0;
    car.bar_flag = 0;
    car.servo_flag = 0;
    car.put_flag = 1;
    car.pick_flag = 0;
    car.uart_flag = 0;
    car.servo_flag_ctrl = 0;
    car.drop_flag = 0;
}

void motor_init(void)
{
    pwm_init(PWM_1, 10000, 0);
    pwm_init(PWM_2, 10000, 0);
    pwm_init(PWM_3, 10000, 0);
    pwm_init(PWM_4, 10000, 0);
    gpio_init(DIR_1, GPO, 0, GPIO_PIN_CONFIG);
    gpio_init(DIR_2, GPO, 0, GPIO_PIN_CONFIG);
    gpio_init(DIR_3, GPO, 0, GPIO_PIN_CONFIG);
    gpio_init(DIR_4, GPO, 0, GPIO_PIN_CONFIG);

    gpio_init(C5, GPO, 0, GPIO_PIN_CONFIG);

    rt_timer_t timer1; // 定时器控制块指针
    timer1 = rt_timer_create("timer1", motorControl_timer1, RT_NULL, 10, RT_TIMER_FLAG_PERIODIC);
    // 首先检查定时器控制器不是空，则启动定时器
    if (timer1 != RT_NULL)
    {
        rt_timer_start(timer1);
    }

    rt_thread_t uart_thread;
    uart_thread = rt_thread_create("uart", uart_trans, RT_NULL, 1024, 25, 100);
    if (uart_thread != RT_NULL)
        rt_thread_startup(uart_thread);

    rt_thread_t servo_thread;
    servo_thread = rt_thread_create("servo", servo_ctrl, RT_NULL, 1024, 9, 5);
    if (servo_thread != RT_NULL)
        rt_thread_startup(servo_thread);

    /************************
    rt_thread_t motor_thread;
    motor_thread = rt_thread_create("motor", motorControl_timer1, RT_NULL, 1024, 1, 10);
    if (motor_thread != RT_NULL)
        rt_thread_startup(motor_thread);
    *************************/

    /*rt_thread_t speedChange_t;
    speedChange_t = rt_thread_create("changeSpeed", speedChange, RT_NULL, 512, 5, 5);
    if(speedChange_t != RT_NULL)
        rt_thread_startup(speedChange_t);*/
    flag_init();
}

void speedChange(void *parameter)
{
    x = 200;
    while (1)
    {
        rt_thread_mdelay(1000);
        x = 0;
    }
}

void motorControl_timer1(void *parameter)
{
    encoder_get();

    // x_mult = 1.688, y_mult = 1.6 for v <= 60
    // x_mult = 1.6, y_mult = 1.1 for v = 200
    // x = 1.65 y = 1.43
    speed_x = (float)speed_2 * 0.5 - (float)speed_3 * 0.5;
    //speed_x = (float)speed_2 * 0.25 - (float)speed_3 * 0.25 + (float)speed_1 * 0.25 -(float)speed_4 * 0.25;
    float32_t mult = 1.63;
    arm_mult_f32(&speed_x, &mult, &speed_x, 1);
    speed_y = (float)speed_1 * 0.5 - (float)speed_2 * 0.5;
    //speed_y = (float)speed_1 * 0.25 - (float)speed_2 * 0.25 - (float)speed_3 * 0.25 + (float)speed_4 * 0.25;
    mult = 1.55; // 1.43 for y = 150
   // int numm = abs(speed_y < 200 ? ((int)(speed_y / 20)) : 9);
    //mult = para_y[numm];
    // uart_putfloat(USART_1, mult), rt_kprintf(", num = %d\n", numm);
    arm_mult_f32(&speed_y, &mult, &speed_y, 1);

    /******
    if (car.motion == 1 || car.motion == 2)
    {
        motionControl();
    }
    if (car.motion == 5)
        to_pic();
    if(car.motion == 6)
        to_pic2();
    if(car.motion == 7)
        pic_mag();
    if(car.motion == 8)
        drop_it();
    if(car.motion == 9)
        go_out();
    if(car.motion == 10)
        go_in();
    if(car.motion == 11)
        protect_back();
    *******/
    switch (car.motion)
    {
    case 1:
        motionControl();
        break;
    case 2:
        motionControl();
        break;
    case 4:
        x = y = z = 0;
        break;
    case 5:
        to_pic();
        break;
    case 6:
        to_pic2();
        break;
    case 7:
        pic_mag();
        break;
    case 8:
        drop_it();
        break;
    case 9:
        go_out();
        break;
    case 10:
        go_in();
        break;
    case 11:
        protect_back();
        break;
    case 12:
        to_bar();
        break;
    case 13:
        wait_get();
        break;
    case 14:
        wait_drop();
        break;
    case 15:
        to_edge();
        break;
    default:
        break;
    }
    // z = get_Theta(0, theta);
    // x = 10;

    target_1 = x + y + z;
    target_2 = x - y + z;
    target_3 = x + y - z;
    target_4 = x - y - z;

    pid_cal(target_1, speed_1, &pid1);
    pid_cal(target_2, speed_2, &pid2);
    pid_cal(-target_3, speed_3, &pid3);
    pid_cal(-target_4, speed_4, &pid4);

    if (pid1.out < 0)
        gpio_set(DIR_1, 0);
    else if (pid1.out > 0)
        gpio_set(DIR_1, 1);
    if (pid2.out < 0)
        gpio_set(DIR_2, 0);
    else if (pid2.out > 0)
        gpio_set(DIR_2, 1);
    if (pid3.out < 0)
        gpio_set(DIR_3, 0);
    else if (pid3.out > 0)
        gpio_set(DIR_3, 1);
    if (pid4.out < 0)
        gpio_set(DIR_4, 0);
    else if (pid4.out > 0)
        gpio_set(DIR_4, 1);

    pwm_duty(PWM_1, abs((int)pid1.out));
    pwm_duty(PWM_2, abs((int)pid2.out));
    pwm_duty(PWM_3, abs((int)pid3.out));
    pwm_duty(PWM_4, abs((int)pid4.out));

    if (abs(pid1.out) < 9900 && abs(pid2.out) < 9900 && abs(pid3.out) < 9900 && abs(pid4.out) < 9900)
        WDOG_Refresh(wdog_base);
}

void uart_trans(void *parameter)
{
    while (1)
    {
        // uart_putfloat(USART_1, x_position), rt_kprintf(","), uart_putfloat(USART_1, y_position), rt_kprintf(","), uart_putfloat(USART_1, index_cur), rt_kprintf(","), uart_putfloat(USART_1, z), rt_kprintf(","), uart_putfloat(USART_1, path_x[index_tar]), rt_kprintf(","), uart_putfloat(USART_1, path_y[index_tar]), rt_kprintf(","), uart_putfloat(USART_1, theta), rt_kprintf("\n");
        // rt_kprintf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", speed_1, speed_2, speed_3, speed_4, target_1, target_2, target_3, target_4, (int)pid1.out, (int)pid2.out, (int)pid3.out,(int)pid4.out);
        float32_t arctan;
        arctan = atan2(path_x[target_cnt][index_tar] - x_position, path_y[target_cnt][index_tar] - y_position);
        // uart_putfloat(USART_1, x_position), rt_kprintf(","), uart_putfloat(USART_1, y_position), rt_kprintf(","), uart_putfloat(USART_1, z), rt_kprintf(","), uart_putfloat(USART_1, path_x[target_cnt][index_tar]), rt_kprintf(","), uart_putfloat(USART_1, path_y[target_cnt][index_tar]), rt_kprintf(","), uart_putfloat(USART_1, car.motion), rt_kprintf(","), uart_putfloat(USART_1, theta), rt_kprintf(","), uart_putfloat(USART_1, target_v), rt_kprintf("\n");

        uart_putfloat(USART_1, x_position), rt_kprintf(","), uart_putfloat(USART_1, y_position), rt_kprintf(","), uart_putfloat(USART_1, target_cnt), rt_kprintf(","), uart_putfloat(USART_1, path_x[tar_cnt][path_tot[tar_cnt]]), rt_kprintf(","), uart_putfloat(USART_1, path_y[tar_cnt][path_tot[tar_cnt]]), rt_kprintf(","), uart_putfloat(USART_1, car.motion), rt_kprintf(","), uart_putfloat(USART_1, px[lsttt]), rt_kprintf(","), uart_putfloat(USART_1, py[lsttt]), rt_kprintf("\n");

        // uart_putfloat(USART_1, speed_3), rt_kprintf(","),uart_putfloat(USART_1, target_3), rt_kprintf(","),uart_putfloat(USART_1, pid3.out), rt_kprintf(","),uart_putfloat(USART_1, pid3.sum_i), rt_kprintf(","), uart_putfloat(USART_1, x_position), rt_kprintf(","), uart_putfloat(USART_1, y_position), rt_kprintf(","), uart_putfloat(USART_1, target_v), rt_kprintf("\n");

        // uart_putfloat(USART_1, x_position),rt_kprintf("\t"),uart_putfloat(USART_1, y_position),rt_kprintf(",%d,%d\n",target_cnt, car.motion);

        // rt_kprintf("%d\t%d\t%d\t%d\t", speed_1, speed_2, speed_3, speed_4),uart_putfloat(USART_1, theta), rt_kprintf("\n");

        float32_t dx, dy, yaw;
        dx = path_x[target_cnt][index_tar] - x_position;
        dy = path_y[target_cnt][index_tar] - y_position;
        yaw = dy == 0 ? (dx > 0 ? 90 : 270) : atan(dx / dy) * 180.0 / 3.14159;
        if (dx < 0 && dy > 0)
            yaw += 360.0;
        if (dx > 0 && dy < 0)
            yaw += 180.0;
        if (dx <= 0 && dy < 0)
            yaw += 180.0;

        // uart_putfloat(USART_1, pic_x-160),rt_kprintf("\t"),uart_putfloat(USART_1, pic_y-120),rt_kprintf("\n");
        // uart_putfloat(USART_1, pic_x-160),rt_kprintf("\t"),uart_putfloat(USART_1, pic_y-120), rt_kprintf("\t"), uart_putfloat(USART_1, std_speed), rt_kprintf("\n");
        // uart_putfloat(USART_1, x_position), rt_kprintf(","), uart_putfloat(USART_1, y_position), rt_kprintf(","), uart_putfloat(USART_1, path_x[target_cnt][index_tar]), rt_kprintf(","), uart_putfloat(USART_1, path_y[target_cnt][index_tar]), rt_kprintf(","), uart_putfloat(USART_1, car.motion), rt_kprintf(","), uart_putfloat(USART_1, x), rt_kprintf(","), uart_putfloat(USART_1, y), rt_kprintf(","), uart_putfloat(USART_1, yaw), rt_kprintf(","), uart_putfloat(USART_1, theta), rt_kprintf("\n");
        // rt_kprintf("%d,%d,%d,%d,", speed_1, speed_2, -speed_3, -speed_4), uart_putfloat(USART_1, target_1), rt_kprintf(","), uart_putfloat(USART_1, target_2), rt_kprintf(","), uart_putfloat(USART_1, target_3), rt_kprintf(","), uart_putfloat(USART_1, target_4), rt_kprintf(","), uart_putfloat(USART_1, pid1.sum_i), rt_kprintf(","), uart_putfloat(USART_1, pid2.sum_i), rt_kprintf(","), uart_putfloat(USART_1, pid3.sum_i), rt_kprintf(","), uart_putfloat(USART_1, pid4.sum_i), rt_kprintf("\n");
        // rt_kprintf("%d,%d,%d,%d,", speed_1, speed_2, -speed_3, -speed_4), uart_putfloat(USART_1, target_1), rt_kprintf(","),uart_putfloat(USART_1, target_2), rt_kprintf(","),uart_putfloat(USART_1, target_3), rt_kprintf(","),uart_putfloat(USART_1, target_4), rt_kprintf(","), uart_putfloat(USART_1, x), rt_kprintf(","), uart_putfloat(USART_1, y), rt_kprintf(","), uart_putfloat(USART_1, z), rt_kprintf("\n");
        // rt_kprintf("%d\n", motion_flag);

        float32_t time = rt_tick_get_millisecond() / 1000;

        char msg[18];
        // time
        // msg[0] = time / 100 + '0';
        // msg[1] = (int)time % 100 / 10 + '0';
        // msg[2] = (int)time % 10 + '0';
        msg[0] = time / 100 + '0';
        msg[1] = (int)time % 100 / 10 + '0';
        msg[2] = (int)time % 10 + '0';
        msg[3] = '.';
        msg[4] = '0';
        msg[5] = '0';
        msg[6] = '0';

        // cor_x
        msg[7] = ' ';
        msg[8] = pos_int[lstttt].x / 10 + '0';
        msg[9] = pos_int[lstttt].x % 10 + '0';

        // cor_y
        msg[10] = ' ';
        msg[11] = pos_int[lstttt].y / 10 + '0';
        msg[12] = pos_int[lstttt].y % 10 + '0';

        // big class
        msg[13] = ' ';
        msg[14] = (class[lstttt] - 1) / 5 + 1 + '0';

        // small class
        msg[15] = ' ';
        msg[16] = (class[lstttt] - 1) % 5 + 1 + '0';
        msg[17] = '\n';

        if (car.send)
            uart_putbuff(USART_1, msg, 18);

        rt_thread_mdelay(100);
    }
}

void to_pic(void)
{
    float32_t yaw_;
    yaw_ = theta * 3.1416 / 180.0;
    x_pos1 += ((speed_x * arm_sin_f32(yaw_) + speed_y * arm_sin_f32(yaw_ + 1.5708)) * 0.01);
    y_pos1 += ((speed_x * arm_cos_f32(yaw_) + speed_y * arm_cos_f32(yaw_ + 1.5708)) * 0.01);

    static float32_t px_, py_;

    px_ = (pic_i - 94) * 0.2585 + 1, py_ = (pic_j - 60) * 0.254 + 10.0;
    int dir_x = px_ > 0 ? 1 : -1, dir_y = py_ > 0 ? 1 : -1;
    if (fabs(px_) < 1e-3)
        dir_x = 0;
    if (fabs(py_) < 1e-3)
        dir_y = 1;
    // px_ += dir_x * 2;
    // py_ += dir_y * 2;

    float32_t dx = px_ - x_pos1, dy = py_ - y_pos1;
    float32_t yaw = dy == 0 ? (dx > 0 ? 90 : 270) : atan(dx / dy) * 180.0 / 3.14159;
    if (dx < 0 && dy > 0)
        yaw += 360.0;
    if (dx > 0 && dy < 0)
        yaw += 180.0;
    if (dx <= 0 && dy < 0)
        yaw += 180.0;

    static int flag = 0;

    float32_t dist = calc_dist(px_, x_pos1, py_, y_pos1);
    if (dist > 1)
    {
        float32_t x_temp, y_temp;
        x_temp = std_speed * arm_cos_f32(yaw / 180.0 * 3.14159);
        y_temp = std_speed * arm_sin_f32(yaw / 180.0 * 3.14159);
        x = x_temp / 1.688, y = y_temp / 1.6;
        z = get_Theta(0, theta);
    }
    else
    {
        x_position = 1.0 * px_std[lsttt];
        y_position = 1.0 * py_std[lsttt];
        x = z = 0;
        y = 0;
        rt_kprintf8("3"), car.send_flag = 1;
        flag = 0;
        car.motion = 13;
    }
    // uart_putfloat(USART_1, px_), rt_kprnitf("\t"), uart_putfloat(USART_1, py_), rt_kprintf("\t"), uart_putfloat(USART_1, x_pos1), rt_kprintf("\t"), uart_putfloat(USART_1, y_pos1), rt_kprintf("\n");
}

void to_pic2(void)
{
    /*******************
    float32_t yaw_;
    yaw_ = theta * (3.1416) / 180;
    x_position += ((speed_x * arm_sin_f32(yaw_) + speed_y * arm_sin_f32(yaw_ + 1.5708)) * 0.01);
    y_position += ((speed_x * arm_cos_f32(yaw_) + speed_y * arm_cos_f32(yaw_ + 1.5708)) * 0.01);
    x = z = 0;
    static int cnt = 0, flag = 0;
    if(cnt > 50 && flag == 0)
        cnt = 0, y *= (-1), flag = 1;
    if(cnt > 100 && flag == 1)
        cnt = 0, y *= (-1);
    cnt++;
    ********************/
    float32_t yaw_;
    yaw_ = theta * 3.1416 / 180.0;
    x_pos1 += ((speed_x * arm_sin_f32(yaw_) + speed_y * arm_sin_f32(yaw_ + 1.5708)) * 0.01);
    y_pos1 += ((speed_x * arm_cos_f32(yaw_) + speed_y * arm_cos_f32(yaw_ + 1.5708)) * 0.01);

    static float32_t px_, py_;
    static int cnt1 = 0;
    px_ = (pic_i - 94) * 0.19302, py_ = (pic_j - 60) * 0.19;
    float32_t dx = px_ - x_pos1, dy = py_ - y_pos1;
    float32_t yaw = dy == 0 ? (dx > 0 ? 90 : 270) : atan(dx / dy) * 180.0 / 3.14159;
    if (dx < 0 && dy > 0)
        yaw += 360.0;
    if (dx > 0 && dy < 0)
        yaw += 180.0;
    if (dx <= 0 && dy < 0)
        yaw += 180.0;
    float32_t dist = calc_dist(px_, x_pos1, py_, y_pos1);
    if (dist > 1)
    {
        float32_t x_temp, y_temp;
        x_temp = std_speed * arm_cos_f32(yaw / 180.0 * 3.14159);
        y_temp = std_speed * arm_sin_f32(yaw / 180.0 * 3.14159);
        x = x_temp / 1.688, y = y_temp / 1.6;
        z = get_Theta(0, theta);
    }

    else
    {
        x_position = 1.0 * px[path[1][cnt1]];
        y_position = 1.0 * py[path[1][cnt1++]];
        x = z = 0;
        y = 0;
        // car.motion = 3;
        car.motion = 7;
    }
}

void pic_mag(void)
{
    car.send = 1;
    x = y = z = 0;
    static int cnt = 0;
    if (cnt < 26)
    {
        pwm_duty(STEER_1, 2400);
        gpio_set(C5, 1);
        cnt++;
    }
    else
    {
        pwm_duty(STEER_1, 3500);
        cnt = 0;
        car.send = 0;
        car.motion = 1;
    }
}

void drop_it(void)
{
    x = y = z = 0;
    static int cnt = 1;
    // if(cnt < 11)
    //     pwm_duty(STEER_1, 5000);
    // if(cnt >= 11 && cnt  < 21)
    {
        gpio_set(C5, 0);
        pwm_duty(STEER_1, 6400), car.motion = 1, cnt = 0;
    }

    cnt++;
}

void go_out(void)
{
    x = 60;
    y = 0;
    z = get_Theta(0, theta);
    float32_t yaw_;
    yaw_ = theta * 3.1416 / 180.0;
    x_position += ((speed_x * arm_sin_f32(yaw_) + speed_y * arm_sin_f32(yaw_ + 1.5708)) * 0.01);
    y_position += ((speed_x * arm_cos_f32(yaw_) + speed_y * arm_cos_f32(yaw_ + 1.5708)) * 0.01);
    if (y_position > -10)
    {
        x = y = z = 0;
        car.motion = 1;
    }
}

void go_in(void)
{
    x = -60;
    y = 0;
    z = get_Theta(0, theta);
    float32_t yaw_;
    yaw_ = theta * 3.1416 / 180.0;
    x_position += ((speed_x * arm_sin_f32(yaw_) + speed_y * arm_sin_f32(yaw_ + 1.5708)) * 0.01);
    y_position += ((speed_x * arm_cos_f32(yaw_) + speed_y * arm_cos_f32(yaw_ + 1.5708)) * 0.01);
    if (y_position < -60)
    {
        x = y = z = 0;
        // rt_kprintf8("44444");
        car.motion = 4;
    }
}

void protect_back(void)
{
    static float32_t x_pos = 0, y_pos = 0;
    static uint8 flag = 0;
    float32_t yaw_;
    yaw_ = theta * 3.1416 / 180.0;
    x_pos += ((speed_x * arm_sin_f32(yaw_) + speed_y * arm_sin_f32(yaw_ + 1.5708)) * 0.01);
    y_pos += ((speed_x * arm_cos_f32(yaw_) + speed_y * arm_cos_f32(yaw_ + 1.5708)) * 0.01);

    switch (flag)
    {
    case 0:
        if (y_pos > -30)
            x = -std_speed;
        else
            flag = 1;
        break;
    case 1:
        if (y_pos < 30)
            x = std_speed;
        else
            flag = 0;
    default:
        break;
    }
    y = 0;
    z = get_Theta(0, theta);

    if (car.is_pic)
        x = y = 0, car.pic_stop = 1, x_pos = y_pos = flag = 0;
}

void to_bar(void)
{
    if (!car.bar_flag)
    {
        x = 10, y = 0, z = get_Theta(0, theta);
    }
    static float32_t x_pos = 0, y_pos = 0;
    if (car.bar_flag == 1)
        x = y = z = 0;
    if (car.bar_flag == 2)
    {
        float32_t yaw_;
        yaw_ = theta * 3.1416 / 180.0;
        x_pos += ((speed_x * arm_sin_f32(yaw_) + speed_y * arm_sin_f32(yaw_ + 1.5708)) * 0.01);
        y_pos += ((speed_x * arm_cos_f32(yaw_) + speed_y * arm_cos_f32(yaw_ + 1.5708)) * 0.01);
        x = 0, z = get_Theta(0, theta);
        if (bar_x > 0)
            y = 15;
        else
            y = -15;
        if (fabs(x_pos - bar_x) < 1)
            x = y = z = 0, car.motion = 4;
    }
}

void wait_get(void)
{
    if (car.uart_flag && car.put_flag)
    {
        car.servo_flag_ctrl = 1;
        if (car.pick_flag)
        {
            car.motion = 1, car.pick_flag = 0, car.uart_flag = 0, car.put_flag = 0;
            if(target_cnt == pic_num)
                car.drop_tail_flag = 1;
        }
    }
}

void wait_drop(void)
{
    x = y = 0;
    if (car.put_flag)
    {
        car.servo_flag_ctrl = 1;
        if (car.drop_flag)
            car.motion = 1, car.drop_flag = 0;
    }
}

void servo_ctrl(void *parameter)
{
    while (1)
    {
        if (car.servo_flag_ctrl)
        {
            switch (car.servo_flag)
            {
            case 0:
                servo_reset();
                break;
            case 1:
                servo_get();
                car.pick_flag = 1;
                servo_reset();
                rt_thread_mdelay(250);
                servo_put_left();
                if (target_cnt < pic_num)
                    servo_reset();
                else
                {
                    rt_thread_mdelay(15);
                    if (car.drop_tail_flag)
                        servo_rote_ctrl(DROP_TAIL), car.drop_tail_flag = 0;
                }
                car.put_flag = 1;
                break;
            case 2:
                servo_get();
                car.pick_flag = 1;
                servo_reset();
                rt_thread_mdelay(250);
                servo_put_tail();
                if (target_cnt < pic_num)
                    servo_reset();
                else
                {
                    rt_thread_mdelay(15);
                    if (car.drop_tail_flag)
                        servo_rote_ctrl(DROP_TAIL), car.drop_tail_flag = 0;
                }
                car.put_flag = 1;
                break;
            case 3:
                servo_get();
                car.pick_flag = 1;
                servo_reset();
                rt_thread_mdelay(250);
                servo_put_right();
                if (target_cnt < pic_num)
                    servo_reset();
                else
                {
                    rt_thread_mdelay(15);
                    if (car.drop_tail_flag)
                        servo_rote_ctrl(DROP_TAIL), car.drop_tail_flag = 0;
                }
                car.put_flag = 1;
                break;
            case 4:
                servo_drop_right();
                car.drop_flag = 1;
                servo_rote_ctrl(DROP_LEFT);
                break;
            case 5:
                servo_drop_tail();
                car.drop_flag = 1;
                servo_rote_ctrl(DROP_RIGHT);
                break;
            case 6:
                servo_drop_left();
                car.drop_flag = 1;
                break;
            }
            car.servo_flag_ctrl = 0;
        }
        rt_thread_mdelay(5);
    }
}

void to_edge(void)
{
    if (!car.bar_flag)
    {
        x = 0, y = 0, z = get_Theta(0, theta);
    }
    static float32_t x_pos = 0, y_pos = 0;
    if (car.bar_flag == 1)
    {
        float32_t yaw_;
        yaw_ = theta * 3.1416 / 180.0;
        x_pos += ((speed_x * arm_sin_f32(yaw_) + speed_y * arm_sin_f32(yaw_ + 1.5708)) * 0.01);
        y_pos += ((speed_x * arm_cos_f32(yaw_) + speed_y * arm_cos_f32(yaw_ + 1.5708)) * 0.01);
        x = 0, z = get_Theta(0, theta);
        if (bar_x > 0)
            y = 15;
        else
            y = -15;
        if (fabs(x_pos - bar_x) < 1)
            car.motion = 10;
    }
}