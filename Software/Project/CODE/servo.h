#ifndef _servo_h
#define _servo_h

#include <headfile.h>
#include <arm_math.h>

#define SERVO_ROTE 		PWM2_MODULE1_CHA_C8
#define SERVO_DROP 		PWM2_MODULE0_CHB_C7
#define SERVO_GET       PWM2_MODULE0_CHA_C6

#define DROP_LEFT   600
#define DROP_RIGHT  1110
#define DROP_TAIL   850
#define PUT_LEFT    840
#define PUT_RIGHT   340
#define PUT_TAIL    1110
#define GET_UP      800
#define GET_PUT     940
#define GET_DOWN    1240    

void servo_init(void);
void servo_reset(void);
void servo_get(void);
void servo_put_left(void);
void servo_put_right(void);
void servo_put_tail(void);
void servo_drop_left(void);
void servo_drop_right(void);
void servo_drop_tail(void);
void servo_rote_ctrl(int tar);

#endif