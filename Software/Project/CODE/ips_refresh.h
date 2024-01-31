#ifndef _IPS_REFRESH_H
#define _IPS_REFRESH_H

#include "SEEKFREE_FONT.h"
#include <headfile.h>

#define PARA_LEN                 8                    //参数数组长度宏定义

extern float float_buff[PARA_LEN];
extern uint16_t motion_flag;
extern rt_mailbox_t ips_mb;
extern uint8_t cur_index;

typedef struct 
{
    uint8_t Cur_Index;//当前索引项
	uint8_t previous;//上一页
	uint8_t next;//下一页
	uint8_t enter;//确认
	uint8_t back;//返回
	void (*current_operation)(uint8_t,uint8_t);//	当前索引执行的函数(界面)    
}menu_t;

//菜单索引
enum
{
	Main_menu=0,
	child1_option,
    child2_option,
    child3_option,

    child1_parameter1option,
    child1_parameter2option,
    child1_parameter3option,
    child1_parameter4option,
    child1_parameter5option,
    child1_parameter6option,
    child1_parameter7option,
    child1_parameter8option,

    child1_parameter1,
    child1_parameter2,
    child1_parameter3,
    child1_parameter4,
    child1_parameter5,
    child1_parameter6,
    child1_parameter7,
    child1_parameter8,

    child2_path_opiton,
    child2_find_coor,
    child2_ant,

    child3_x,
    child3_y,
    child3_z,
    child3_run,

    child3_x_change,
    child3_y_change,
    child3_z_change,
};

//按键索引值
enum
{
	KEY_PREVIOUS=2,
	KEY_ENTER,
	KEY_NEXT,
	KEY_BACK
};

void IPS_INIT(void);
void ips114_displayimage032_zoomT(uint8 *p, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height);

extern rt_sem_t camera_sem;

#endif