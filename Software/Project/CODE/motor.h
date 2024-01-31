#ifndef _motor_h
#define _motor_h

#include <headfile.h>
#include <arm_math.h>

//电机序号定义         3（左前）      2（右前）
                                                                
//                    4（左后）      1（右后）
#define DIR_1				D14
#define DIR_3				D1
#define DIR_2               D0
#define DIR_4               D15

#define PWM_4				PWM1_MODULE0_CHB_D13
#define PWM_3				PWM2_MODULE3_CHB_D3 
#define PWM_2				PWM2_MODULE3_CHA_D2
#define PWM_1				PWM1_MODULE0_CHA_D12
#define STEER_1             PWM2_MODULE0_CHA_C6 

typedef struct
{
    uint8_t motion;
    int8_t uart;
    uint8_t pic_flag;
    uint8_t send_flag;
    uint8_t twice_flag;
    uint16_t to_pic_cnt;
    uint8_t is_pic;
    uint8_t pic_stop;
    uint8_t send;
    uint8_t bar_flag;
    uint8_t servo_flag;
    uint8_t put_flag;
    uint8_t pick_flag;
    uint8_t uart_flag;
    uint8_t servo_flag_ctrl;
    uint8_t drop_flag;
    uint8_t drop_tail_flag;
}status;

extern status car;

void motorControl_timer1(void *parameter);

extern float32_t target_1, target_2, target_3, target_4;
extern float32_t x, y, z;
extern float32_t x_pos1, y_pos1;

#endif