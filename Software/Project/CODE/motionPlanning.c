#include "motionPlanning.h"

float32_t x_para[6], y_para[6];                                       //轨迹规划中x，y路径分别对应的参数（5次曲线规划）
float32_t path_x[21 * 3][NUM], path_y[21 * 3][NUM], path_yaw[21 * 3]; //轨迹x，y坐标及yaw角
float32_t target_x[10], target_y[10];
uint16_t index_cur = 0, index_tar = 0;
uint16_t target_cnt = 0; //当前目标点（大方向）   e.g. path_x[target_cnt][i]  即可检索一个轨迹点
uint16_t tar_cnt = 0;
float32_t temp_;
uint16_t target_num;
float32_t pic_x, pic_y, pic_yaw;
float32_t target_v;

float32_t calc_dist(float32_t x0, float32_t x1, float32_t y0, float32_t y1); //计算距离
uint16_t get_tar(float32_t ix, float32_t iy);
int go_point(float x0, float y0, float x1, float y1);

void motionControl(void)
{
    float32_t yaw_;
    yaw_ = theta * (3.1416) / 180;
    x_position += ((speed_x * arm_sin_f32(yaw_) + speed_y * arm_sin_f32(yaw_ + 1.5708)) * 0.01);
    y_position += ((speed_x * arm_cos_f32(yaw_) + speed_y * arm_cos_f32(yaw_ + 1.5708)) * 0.01);

    float32_t ix, iy, iyaw;
    ix = path_x[target_cnt][index_cur];
    iy = path_y[target_cnt][index_cur];
    iyaw = path_yaw[target_cnt];

    /*if(y_position < 400)
        x = 60;
    else
        x = 0;*/
    if(go_point(0, 0, 200, 0))
        car.motion = 4;

/***
    if(target_cnt < pic_num + 3)
        if (go_point(0, 0, px[lsttt], py[lsttt]))
        {
            if (target_cnt == pic_num)
            {
                target_cnt++;
                car.servo_flag = 5;         //The right side action--fruits, drop tail
                car.motion = 14;
                ACO_to_T();
                return;
            }
            if (target_cnt == pic_num + 1)
            {
                target_cnt++;
                car.servo_flag = 4;         //The top side action--trans  ,  drop right
                car.motion = 14;
                ACO_to_L();
                return;
            }
            if (target_cnt == pic_num + 2)
            {
                target_cnt++;
                car.servo_flag = 6;         //The left side action--animals, drop left
                car.motion = 14;
                return;
            }

            x = y = z = target_v = 0;
            target_cnt++, car.motion = 3;
            car.pic_flag = car.send_flag = 0;
            return;
        }
    if(target_cnt == pic_num + 3)
        car.motion = 15, rt_kprintf8("4");
        ***/
}

float32_t calc_dist(float32_t x0, float32_t x1, float32_t y0, float32_t y1)
{
    float32_t ans, sqrt;
    ans = (x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1);
    arm_sqrt_f32(ans, &sqrt);
    return sqrt;
}

uint16_t get_tar(float32_t ix, float32_t iy)
{
    //寻找下一目标点，大于参考距离设为目标点
    float32_t ref_dist;
    ref_dist = 8; //+ 0.04 * fabs(target_v);
    for (int i = index_cur; i <= path_tot[target_cnt]; i++)
    {
        if (calc_dist(ix, path_x[target_cnt][i], iy, path_y[target_cnt][i]) > ref_dist)
            return i;
    }
    return path_tot[target_cnt];
}

float32_t restrict_theta(float32_t a)
{
    return a > 360 ? (a - 360) : a;
}

float32_t get_Theta(float32_t tar, float32_t cur)
{
    float32_t ans, theta1 = fabs(tar - cur), theta2 = 360 - fabs(tar - cur);
    ans = theta1 < theta2 ? theta1 : theta2;
    if (fabs(restrict_theta(tar + ans) - theta) < 1e-4)
        return ans;
    else
        return -1.0 * ans;
}

int cnt_v = 0;
int go_point(float x0, float y0, float x1, float y1)
{
    float dist_ = calc_dist(path_x[tar_cnt][0], path_x[tar_cnt][path_tot[tar_cnt]], path_y[tar_cnt][0], path_y[tar_cnt][path_tot[tar_cnt]]);
    temp_ = calc_dist(x_position, x1, y_position, y1);

/*
    if (dist_ < 176)
    {
        int tot = sqrt(dist_ / 100 / 3) * 100;
        if (cnt_v < tot - 6)
            target_v += 3;
        else
        {
            target_v -= 2;
            if (target_v < 60)
                target_v = 60;
        }
    }
    else
    {
        int tot = (dist_ - 176) / 230 * 100;
        if (cnt_v < 76)
            target_v += 3;
        else if (cnt_v >= 76 && cnt_v < 66 + tot)
            target_v = 230;
        else
        {
            target_v -= 2;
            if (target_v < 60)
                target_v = 60;
        }
    }
    if (dist_ < 60)
        target_v = 60;
*/
/*
    if (dist_ < 212)
    {
        int tot = sqrt(dist_ / 100 / 2.5) * 100;
        if (cnt_v < tot)
            target_v += 2.5;
        else
        {
            target_v -= 2;
            if (target_v < 60)
                target_v = 60;
        }
    }
    else
    {
        int tot = (dist_ - 212) / 230 * 100;
        if (cnt_v < 92)
            target_v += 2.5;
        else if (cnt_v >= 92 && cnt_v < 92 + tot)
            target_v = 230;
        else
        {
            target_v -= 2;
            if (target_v < 60)
                target_v = 60;
        }
    }
    if (dist_ < 60)
        target_v = 60;
*/
    
    target_v = 60;
    //++cnt_v;
    float32_t dx, dy, yaw;
    dx = x1 - x_position;
    dy = y1 - y_position;
    yaw = dy == 0 ? (dx > 0 ? 90 : 270) : atan(dx / dy) * 180.0 / 3.14159;
    if (dx < 0 && dy > 0)
        yaw += 360.0;
    if (dx > 0 && dy < 0)
        yaw += 180.0;
    if (dx <= 0 && dy < 0)
        yaw += 180.0;
    pid_cal(yaw, 0, &pid_yaw);
    float32_t x_temp, y_temp;
    x_temp = target_v * arm_cos_f32(pid_yaw.out / 180.0 * 3.14159);
    y_temp = target_v * arm_sin_f32(pid_yaw.out / 180.0 * 3.14159);
    static float last_x = 0, last_y = 0;
    last_x = last_x * 0.6 + x_temp * 0.4;
    last_y = last_y * 0.6 + y_temp * 0.4;
    x = last_x / 1.688;
    y = last_y / 1.6;
    z = get_Theta(0, theta);

    if (temp_ < 4)
    {
      return 1;
    }
    return 0;
}