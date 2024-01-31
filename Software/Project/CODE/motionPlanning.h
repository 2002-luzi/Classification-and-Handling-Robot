#ifndef _MOTION_H
#define _MOTION_H

#include <headfile.h>
#include <arm_math.h>

#define NUM 103

void motionControl(void);
void path_init(void);
float32_t get_Theta(float32_t tar, float32_t cur);
float32_t calc_dist(float32_t x0, float32_t x1, float32_t y0, float32_t y1);

extern float32_t path_x[21 * 3][NUM], path_y[21 * 3][NUM], path_yaw[21 * 3], temp_;
extern uint16_t index_cur, index_tar, target_num, target_cnt, tar_cnt;
extern float32_t pic_x, pic_y, pic_yaw;
extern float32_t target_v;

#endif