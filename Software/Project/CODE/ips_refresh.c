#include "ips_refresh.h"

#define PARA_FLASH_SECTOR (FLASH_SECTOR_NUM - 1) //扇区宏定义，最后一个扇区
#define PARA_FLASH_SECTOR_PAGE (0)               //扇区页宏定义

rt_mailbox_t ips_mb;                              //屏幕邮箱
rt_sem_t camera_sem;                              //摄像头队列
uint8_t cur_index = child2_find_coor;             //当前页面索引值
uint8_t last_index = Main_menu;                   //上一个页面索引值
uint8_t key_val;                                  //按键值
void (*current_operation_func)(uint8_t, uint8_t); //定义一个函数指针
uint32 int_buff[PARA_LEN];
float float_buff[PARA_LEN];
int16 x_temp = 0, y_temp = 0, z_temp = 0;
uint16_t motion_flag = 0;

// uint8_t tmp[MT9V03X_CSI_H][MT9V03X_CSI_W];

void GUI_refresh_timer2(void *parameter);
void ips_mb_recv(void *parameter);
void Main_UI(uint8_t page_index, uint8_t key_value);
void Main_menu_UI(uint8_t page_index, uint8_t key_value);
void child1_UI(uint8_t page_index, uint8_t key_value);
void child3_UI(uint8_t page_index, uint8_t key_value);
void child1_change(uint8_t page_index, uint8_t key_value);
void child3_change(uint8_t page_index, uint8_t key_value);
void child2_showimg(uint8_t page_index, uint8_t key_value);
void child2_find_coor_UI(uint8_t page_index, uint8_t key_value);
void child2_ant_UI(uint8_t page_index, uint8_t key_value);
void show_red_corner(node LS);
void show_red_node(int x, int y);

//用于描述各菜单之间的上下级关系
//顺序应与菜单索引枚举类型中的顺序一致
//关系如下定义  Cur_Index,previous,next,enter,back,(*current_operation)(u8,u8)
menu_t table[30] = {
    //主界面
    {Main_menu, Main_menu, Main_menu, child1_option, Main_menu, Main_UI},

    //主菜单
    {child1_option, child3_option, child2_option, child1_parameter1option, Main_menu, Main_menu_UI},
    {child2_option, child1_option, child3_option, child2_path_opiton, Main_menu, Main_menu_UI},
    {child3_option, child2_option, child1_option, child3_x, Main_menu, Main_menu_UI},

    // child1菜单
    {child1_parameter1option, child1_parameter8option, child1_parameter2option, child1_parameter1, child1_option, child1_UI},
    {child1_parameter2option, child1_parameter1option, child1_parameter3option, child1_parameter2, child1_option, child1_UI},
    {child1_parameter3option, child1_parameter2option, child1_parameter4option, child1_parameter3, child1_option, child1_UI},
    {child1_parameter4option, child1_parameter3option, child1_parameter5option, child1_parameter4, child1_option, child1_UI},
    {child1_parameter5option, child1_parameter4option, child1_parameter6option, child1_parameter5, child1_option, child1_UI},
    {child1_parameter6option, child1_parameter5option, child1_parameter7option, child1_parameter6, child1_option, child1_UI},
    {child1_parameter7option, child1_parameter6option, child1_parameter8option, child1_parameter7, child1_option, child1_UI},
    {child1_parameter8option, child1_parameter7option, child1_parameter1option, child1_parameter8, child1_option, child1_UI},

    // child1 参数修改
    {child1_parameter1, child1_parameter1, child1_parameter1, child1_parameter1, child1_parameter1option, child1_change},
    {child1_parameter2, child1_parameter2, child1_parameter2, child1_parameter2, child1_parameter2option, child1_change},
    {child1_parameter3, child1_parameter3, child1_parameter3, child1_parameter3, child1_parameter3option, child1_change},
    {child1_parameter4, child1_parameter4, child1_parameter4, child1_parameter4, child1_parameter4option, child1_change},
    {child1_parameter5, child1_parameter5, child1_parameter5, child1_parameter5, child1_parameter5option, child1_change},
    {child1_parameter6, child1_parameter6, child1_parameter6, child1_parameter6, child1_parameter6option, child1_change},
    {child1_parameter7, child1_parameter7, child1_parameter7, child1_parameter7, child1_parameter7option, child1_change},
    {child1_parameter8, child1_parameter8, child1_parameter8, child1_parameter8, child1_parameter8option, child1_change},

    // child2界面，显示规划路径
    // Cur_Index,previous,next,enter,back,(*current_operation)(u8,u8)

    {child2_path_opiton, child2_path_opiton, child2_path_opiton, child2_find_coor, child2_option, child2_showimg},
    {child2_find_coor, child2_find_coor, child2_find_coor, child2_find_coor, child2_path_opiton, child2_find_coor_UI},
    {child2_ant, child2_ant, child2_ant, child2_ant, child2_find_coor, child2_ant_UI},

    // child3菜单
    {child3_x, child3_run, child3_y, child3_x_change, child3_option, child3_UI},
    {child3_y, child3_x, child3_z, child3_y_change, child3_option, child3_UI},
    {child3_z, child3_y, child3_run, child3_z_change, child3_option, child3_UI},
    {child3_run, child3_z, child3_x, child3_run, child3_option, child3_UI},

    // child3 修改x，y，z速度
    {child3_x_change, child3_x_change, child3_x_change, child3_x_change, child3_x, child3_change},
    {child3_y_change, child3_y_change, child3_y_change, child3_y_change, child3_y, child3_change},
    {child3_z_change, child3_z_change, child3_z_change, child3_z_change, child3_z, child3_change},

};

void IPS_INIT(void)
{
    rt_timer_t timer2;
    ips114_init();
    timer2 = rt_timer_create("timer2", GUI_refresh_timer2, RT_NULL, 180, RT_TIMER_FLAG_PERIODIC);
    gpio_init(B15, GPI, 0, GPIO_PIN_CONFIG);
    gpio_init(B14, GPI, 0, GPIO_PIN_CONFIG);
    gpio_init(B16, GPI, 0, GPIO_PIN_CONFIG);
    gpio_init(B17, GPI, 0, GPIO_PIN_CONFIG);

    flash_init();

    flash_read_page(PARA_FLASH_SECTOR, PARA_FLASH_SECTOR_PAGE, int_buff, PARA_LEN);
    for (int i = 0; i < PARA_LEN; i++)
        float_buff[i] = uint32_conversion_float(int_buff[i]);
    pid1.p = float_buff[0];
    pid1.i = float_buff[1];
    pid2.p = float_buff[2];
    pid2.i = float_buff[3];
    pid3.p = float_buff[4];
    pid3.i = float_buff[5];
    pid4.p = float_buff[6];
    pid4.i = float_buff[7];

    pid1.p = pid2.p = pid3.p = pid4.p = 45;
    pid1.i = pid2.i = pid3.i = pid4.i = 13;

    rt_thread_t ips_mb_recv_t; //屏幕邮箱线程
    ips_mb = rt_mb_create(
        "ips_mb",        //邮箱名称
        1,               //缓冲区邮件数
        RT_IPC_FLAG_FIFO //先进先出
    );
    ips_mb_recv_t = rt_thread_create(
        "ips_mb_recv_thread", //线程名称
        ips_mb_recv,          //线程入口函数
        RT_NULL,              //线程参数
        8000,                 //栈空间大小
        31,                   //线程优先级
        1000);                //时间片
    if (ips_mb_recv_t != NULL)
        rt_thread_startup(ips_mb_recv_t);

    if (timer2 != RT_NULL)
    {
        rt_timer_start(timer2);
    }
    // memset(float_buff, 0, sizeof float_buff);
    camera_sem = rt_sem_create("camera", 0, RT_IPC_FLAG_FIFO);

    gpio_init(C4, GPO, 0, GPIO_PIN_CONFIG);
}

void GUI_refresh_timer2(void *parameter) //刷新界面，有输入才刷新，160ms定时中断
{
    uint8 key_15, key_16, key_14, key_17;

    key_15 = gpio_get(B15);
    key_16 = gpio_get(B16);
    key_14 = gpio_get(B14);
    key_17 = gpio_get(B17);
    if (!key_15 || !key_16 || !key_14 || !key_17)
    {
        if (!key_15)
            key_val = KEY_PREVIOUS;
        if (!key_14)
            key_val = KEY_NEXT;
        if (!key_16)
            key_val = KEY_BACK;
        if (!key_17)
            key_val = KEY_ENTER;
        static int flag_ = 0;
        if (!flag_)
            flag_ = 1;
        rt_mb_send(ips_mb, key_val);
    }
    // if (cur_index == child2_path_opiton || cur_index == child2_ant)
    //   rt_mb_send(ips_mb, 0);
    // if (cur_index == child2_ant)
    //    rt_mb_send(ips_mb, 0);
}

/*****************************************
 * 屏幕邮箱线程函数
 *****************************************/
void ips_mb_recv(void *parameter)
{
    uint32 data;
    while (1)
    {
        if (rt_mb_recv(
                ips_mb,             //邮箱控制块
                &data,              //接受邮箱的字符串 大小32bit
                RT_WAITING_FOREVER) //一直等待
            == RT_EOK)
        {
            uint8_t key_val_mb = (uint8_t)data;
            last_index = cur_index; //更新上一界面索引值
            switch (key_val_mb)     //更新索引值
            {
            case KEY_PREVIOUS:
                cur_index = table[cur_index].previous;
                break;
            case KEY_ENTER:
                cur_index = table[cur_index].enter;
                break;
            case KEY_NEXT:
                cur_index = table[cur_index].next;
                break;
            case KEY_BACK:
                cur_index = table[cur_index].back;
                break;
            default:
                break;
            }
            if (cur_index != child2_path_opiton)
                ips114_clear(BLACK);

            current_operation_func = table[cur_index].current_operation;
            (*current_operation_func)(cur_index, key_val_mb); //执行当前索引对应的函数
        }
    }
}

void Main_UI(uint8_t page_index, uint8_t key_value)
{
    ips114_showstr(80, 3, "Smart Car");
}

void Main_menu_UI(uint8_t page_index, uint8_t key_value)
{
    ips114_showstr(90, 0, "Menu");
    ips114_showstr(20, 2, "Change Parameters");
    ips114_showstr(20, 4, "Show Identified Results");
    ips114_showstr(20, 6, "Run the Demo");

    switch (cur_index)
    {
    case child1_option:
        ips114_showstr(0, 2, "->");
        break;
    case child2_option:
        ips114_showstr(0, 4, "->");
        break;
    case child3_option:
        ips114_showstr(0, 6, "->");
        break;
    }
    if (key_value == KEY_BACK)
    {
        for (int i = 0; i < PARA_LEN; i++)
            int_buff[i] = float_conversion_uint32(float_buff[i]);
        if (flash_check(PARA_FLASH_SECTOR, PARA_FLASH_SECTOR_PAGE))
        {
            int8_t status;
            status = flash_erase_sector(PARA_FLASH_SECTOR);
            if (status)
                while (1)
                    ;
        }
        uint8 status;
        status = flash_page_program(PARA_FLASH_SECTOR, PARA_FLASH_SECTOR_PAGE, int_buff, PARA_LEN);
        if (status)
            rt_kprintf("Flash program failed!\n");
        flash_read_page(PARA_FLASH_SECTOR, PARA_FLASH_SECTOR_PAGE, int_buff, PARA_LEN);
    }
}

void child1_UI(uint8_t page_index, uint8_t key_value)
{
    ips114_showstr(20, 0, "P1");
    ips114_showfloat(50, 0, float_buff[0], 2, 1);
    ips114_showstr(20, 2, "I1");
    ips114_showfloat(50, 2, float_buff[1], 2, 1);
    ips114_showstr(20, 4, "P2");
    ips114_showfloat(50, 4, float_buff[2], 2, 1);
    ips114_showstr(20, 6, "I2");
    ips114_showfloat(50, 6, float_buff[3], 2, 1);
    ips114_showstr(120, 0, "P3");
    ips114_showfloat(150, 0, float_buff[4], 2, 1);
    ips114_showstr(120, 2, "I3");
    ips114_showfloat(150, 2, float_buff[5], 2, 1);
    ips114_showstr(120, 4, "P4");
    ips114_showfloat(150, 4, float_buff[6], 2, 1);
    ips114_showstr(120, 6, "I4");
    ips114_showfloat(150, 6, float_buff[7], 2, 1);

    switch (cur_index)
    {
    case child1_parameter1option:
        ips114_showstr(0, 0, "->");
        break;
    case child1_parameter2option:
        ips114_showstr(0, 2, "->");
        break;
    case child1_parameter3option:
        ips114_showstr(0, 4, "->");
        break;
    case child1_parameter4option:
        ips114_showstr(0, 6, "->");
        break;
    case child1_parameter5option:
        ips114_showstr(100, 0, "->");
        break;
    case child1_parameter6option:
        ips114_showstr(100, 2, "->");
        break;
    case child1_parameter7option:
        ips114_showstr(100, 4, "->");
        break;
    case child1_parameter8option:
        ips114_showstr(100, 6, "->");
        break;
    }
    if (key_value == KEY_BACK)
    {
        pid1.p = float_buff[0];
        pid1.i = float_buff[1];
        pid2.p = float_buff[2];
        pid2.i = float_buff[3];
        pid3.p = float_buff[4];
        pid3.i = float_buff[5];
        pid4.p = float_buff[6];
        pid4.i = float_buff[7];
    }
}

/***************************************************************
 * child1_change: change pid parameters
 * ********************************************************
 * ****************************************************
 * ********************************************************
 * ********************************************************
 * ********************************************************
 * **********************************************************
 * **************************************
 * *************************************************************/
void child1_change(uint8_t page_index, uint8_t key_value)
{
    ips114_showstr(20, 0, "P1");
    ips114_showfloat(50, 0, float_buff[0], 2, 1);
    ips114_showstr(20, 2, "I1");
    ips114_showfloat(50, 2, float_buff[1], 2, 1);
    ips114_showstr(20, 4, "P2");
    ips114_showfloat(50, 4, float_buff[2], 2, 1);
    ips114_showstr(20, 6, "I2");
    ips114_showfloat(50, 6, float_buff[3], 2, 1);
    ips114_showstr(120, 0, "P3");
    ips114_showfloat(150, 0, float_buff[4], 2, 1);
    ips114_showstr(120, 2, "I3");
    ips114_showfloat(150, 2, float_buff[5], 2, 1);
    ips114_showstr(120, 4, "P4");
    ips114_showfloat(150, 4, float_buff[6], 2, 1);
    ips114_showstr(120, 6, "I4");
    ips114_showfloat(150, 6, float_buff[7], 2, 1);
    switch (page_index)
    {
    case child1_parameter1:
        ips114_showstr(0, 0, "@");
        if (key_value == KEY_PREVIOUS)
            float_buff[0] += 0.2, ips114_showfloat(50, 0, float_buff[0], 2, 1);
        else if (key_value == KEY_NEXT)
            float_buff[0] -= 0.2, ips114_showfloat(50, 0, float_buff[0], 2, 1);
        break;
    case child1_parameter2:
        ips114_showstr(0, 2, "@");
        if (key_value == KEY_PREVIOUS)
            float_buff[1] += 0.2, ips114_showfloat(50, 2, float_buff[1], 2, 1);
        else if (key_value == KEY_NEXT)
            float_buff[1] -= 0.2, ips114_showfloat(50, 2, float_buff[1], 2, 1);
        break;
    case child1_parameter3:
        ips114_showstr(0, 4, "@");
        if (key_value == KEY_PREVIOUS)
            float_buff[2] += 0.2, ips114_showfloat(50, 4, float_buff[2], 2, 1);
        else if (key_value == KEY_NEXT)
            float_buff[2] -= 0.2, ips114_showfloat(50, 4, float_buff[2], 2, 1);
        break;
    case child1_parameter4:
        ips114_showstr(0, 6, "@");
        if (key_value == KEY_PREVIOUS)
            float_buff[3] += 0.2, ips114_showfloat(50, 6, float_buff[3], 2, 1);
        else if (key_value == KEY_NEXT)
            float_buff[3] -= 0.2, ips114_showfloat(50, 6, float_buff[3], 2, 1);
        break;
    case child1_parameter5:
        ips114_showstr(100, 0, "@");
        if (key_value == KEY_PREVIOUS)
            float_buff[4] += 0.2, ips114_showfloat(150, 0, float_buff[4], 2, 1);
        else if (key_value == KEY_NEXT)
            float_buff[4] -= 0.2, ips114_showfloat(150, 0, float_buff[4], 2, 1);
        break;
    case child1_parameter6:
        ips114_showstr(100, 2, "@");
        if (key_value == KEY_PREVIOUS)
            float_buff[5] += 0.2, ips114_showfloat(150, 2, float_buff[5], 2, 1);
        else if (key_value == KEY_NEXT)
            float_buff[5] -= 0.2, ips114_showfloat(150, 2, float_buff[5], 2, 1);
        break;
    case child1_parameter7:
        ips114_showstr(100, 4, "@");
        if (key_value == KEY_PREVIOUS)
            float_buff[6] += 0.2, ips114_showfloat(150, 4, float_buff[6], 2, 1);
        else if (key_value == KEY_NEXT)
            float_buff[6] -= 0.2, ips114_showfloat(150, 4, float_buff[6], 2, 1);
        break;
    case child1_parameter8:
        ips114_showstr(100, 6, "@");
        if (key_value == KEY_PREVIOUS)
            float_buff[7] += 0.2, ips114_showfloat(150, 6, float_buff[7], 2, 1);
        else if (key_value == KEY_NEXT)
            float_buff[7] -= 0.2, ips114_showfloat(150, 6, float_buff[7], 2, 1);
        break;
    }
}

void child3_UI(uint8_t page_index, uint8_t key_value)
{
    ips114_showstr(80, 1, "x");
    ips114_showint16(120, 1, x_temp);
    ips114_showstr(80, 3, "y");
    ips114_showint16(120, 3, y_temp);
    ips114_showstr(80, 5, "z");
    ips114_showint16(120, 5, z_temp);
    ips114_showstr(100, 7, "Run!");
    switch (page_index)
    {
    case child3_x:
        ips114_showstr(60, 1, "->");
        break;
    case child3_y:
        ips114_showstr(60, 3, "->");
        break;
    case child3_z:
        ips114_showstr(60, 5, "->");
        break;
    case child3_run:
        ips114_showstr(80, 7, "->");
        if (key_value == KEY_ENTER)
        {
            rt_thread_mdelay(500);
            x = x_temp;
            y = y_temp;
            z = z_temp;
        }
    default:
        break;
    }
}

void child3_change(uint8_t page_index, uint8_t key_value)
{
    ips114_showstr(80, 1, "x");
    ips114_showint16(120, 1, x_temp);
    ips114_showstr(80, 3, "y");
    ips114_showint16(120, 3, y_temp);
    ips114_showstr(80, 5, "z");
    ips114_showint16(120, 5, z_temp);

    switch (page_index)
    {
    case child3_x_change:
        ips114_showstr(60, 1, "@");
        if (key_value == KEY_PREVIOUS)
            x_temp += 10;
        else if (key_value == KEY_NEXT)
            x_temp -= 10;
        ips114_showint16(120, 1, x_temp);
        break;
    case child3_y_change:
        ips114_showstr(60, 3, "@");
        if (key_value == KEY_PREVIOUS)
            y_temp += 10;
        else if (key_value == KEY_NEXT)
            y_temp -= 10;
        ips114_showint16(120, 3, y_temp);
        break;
    case child3_z_change:
        ips114_showstr(60, 5, "@");
        if (key_value == KEY_PREVIOUS)
            z_temp += 10;
        else if (key_value == KEY_NEXT)
            z_temp -= 10;
        ips114_showint16(120, 5, z_temp);
        break;
    default:
        break;
    }
}

void child2_showimg(uint8_t page_index, uint8_t key_value)
{
    /**************
    uint32 begin, end;

    begin = rt_tick_get_millisecond();
    find_pic();
    end = rt_tick_get_millisecond();

    rt_kprintf("number of pictures: %d\n", pic_num);
    for (int i = 0; i < pic_num; ++i)
        rt_kprintf("%d-th: (%d, %d)\n", i + 1, pos_int[i].x, pos_int[i].y);
    ***************/
}

void child2_find_coor_UI(uint8_t page_index, uint8_t key_value)
{
    static int flag = 0;
    if (!flag)
    {
        
        flag = 1;
        /*
        uint32 begin, end;
        rt_sem_take(camera_sem, RT_WAITING_FOREVER);
        for (int i = 0; i < MT9V03X_CSI_W; ++i)
            for (int j = 0; j < MT9V03X_CSI_H; ++j)
                img_origin[i][j] = mt9v03x_csi_image[MT9V03X_CSI_H - 1 - j][i];

        begin = rt_tick_get_millisecond();
        find_pic();
        end = rt_tick_get_millisecond();
        // rt_kprintf("%d\n", task_num);
        for (int i = 0; i < pic_num; i++)
            px[i] = pos_int[i].x, py[i] = pos_int[i].y;

        // rt_kprintf("number of pictures: %d\n", pic_num);
        for (int i = 0; i < pic_num; ++i)
             rt_kprintf("%d-th: (%d, %d)\n", i + 1, pos_int[i].x, pos_int[i].y)
            ;
        */
        //rt_kprintf("pic_num = %d\n", pic_num);
         
        pic_num = 13;
        pos_int[0].x = 16, pos_int[0].y = 14;
        pos_int[1].x = 13, pos_int[1].y = 9;
        pos_int[2].x = 20, pos_int[2].y = 16;
        pos_int[3].x = 14, pos_int[3].y = 19;
        pos_int[4].x = 23, pos_int[4].y = 6;
        pos_int[5].x = 5, pos_int[5].y = 5;
        pos_int[6].x = 3, pos_int[6].y = 14;
        pos_int[7].x = 9, pos_int[7].y = 22;
        pos_int[8].x = 26, pos_int[8].y = 23;
        pos_int[9].x = 2, pos_int[9].y = 2;
        pos_int[10].x = 33, pos_int[10].y = 2;
        pos_int[11].x = 4, pos_int[11].y = 24;
        pos_int[12].x = 33, pos_int[12].y = 21;
        for (int i = 0; i < pic_num; i++)
            px[i] = pos_int[i].x, py[i] = pos_int[i].y;
        
        gpio_set(C4, 1);
        // if (TASK_TAG)
        //     to_first();
        // else
        //     to_first(), rt_thread_mdelay(200);
        ACO_to_first();
        rt_thread_mdelay(300);
        gpio_set(C4, 0);
        car.motion = 1;
    }
    /***
    pic_num = 13;
    pos_int[0].x = 16, pos_int[0].y = 14;
    pos_int[1].x = 13, pos_int[1].y = 9;
    pos_int[2].x = 20, pos_int[2].y = 16;
    pos_int[3].x = 14, pos_int[3].y = 19;
    pos_int[4].x = 23, pos_int[4].y = 6;
    pos_int[5].x = 5, pos_int[5].y = 5;
    pos_int[6].x = 3, pos_int[6].y = 14;
    pos_int[7].x = 9, pos_int[7].y = 22;
    pos_int[8].x = 26, pos_int[8].y = 23;
    pos_int[9].x = 2, pos_int[9].y = 2;
    pos_int[10].x = 33, pos_int[10].y = 2;
    pos_int[11].x = 4, pos_int[11].y = 24;
    pos_int[12].x = 33, pos_int[12].y = 21;
    for (int i = 0; i < pic_num; i++)
        px[i] = pos_int[i].x, py[i] = pos_int[i].y;
    ****/

    // rt_kprintf("time %d\n", end - begin);

    /****************************************
    for(int i = 0; i < MT9V03X_CSI_H; i++)
        for(int j = 0; j < MT9V03X_CSI_W; j++)
            tmp[i][j] = mt9v03x_csi_image[i][j];
    for(int i = 0; i < MT9V03X_CSI_H; i++)
    {
        for(int j = 0; j < MT9V03X_CSI_W; j++)
            rt_kprintf("%d ", tmp[i][j]);
        rt_kprintf("\n");
    }
    rt_kprintf("-------------------------\n-----------------------\n---------------------\n\n\n\n\n\n\n\n");
    ****************************************/

    /**************************************
    rt_sem_take(camera_sem, RT_WAITING_FOREVER);
    get_pic();
    **************************************/
}

void child2_ant_UI(uint8_t page_index, uint8_t key_value)
{
    /****************** ant *****************
    static uint8_t flag_ = 0;
    if (!flag_)
    {
        uint32 begin, end;

        begin = rt_tick_get_millisecond();
        run_twice1(); //, run_twice2();
        end = rt_tick_get_millisecond();

        for (int j = 0; j < pic_num + 1; j++)
        {
            for (int i = 0; i < path_tot[j]; i++)
            {
                float32_t x_point, y_point;
                x_point = path_x[j][i] * 239 / 700;
                y_point = 134 - (path_y[j][i] * 134 / 500);
                ips114_drawpoint((uint16)x_point, (uint16)y_point, WHITE);
            }
        }
        for (int i = 0; i < pic_num; i++)
            show_red_node(px[i], py[i]);
        if (!car.motion)
            car.motion = motion_flag = 1;

        if (key_value == KEY_BACK)
            car.motion = 0;
        flag_ = 1;
    }
    if(flag_ == 1)
    {
        if (car.twice_flag)
        {
            run_twice2();
            car.motion = 6;
            flag_ = 2;
            rt_kprintf("!!!!!!!!!!!!!\n!!!!!!!!!!!\n");
        }
    }
    *****************/

    /************** debug *************
    static uint8_t flag_ = 0;
    if (!flag_)
    {
        int num_path = 0;
        //make_path(0, 0, 300, 0, num_path, 15), ++num_path;
        //make_path(300, 0, 300, 200, num_path, 15), ++num_path;
        //make_path(300, 200, 0, 200, num_path, 15), ++num_path;
        //make_path(0, 200, 0, 0, num_path, 15), ++num_path;

        int tt = 10, tag = 0;
        int curx = 0, cury = 0, nxtx = 0, nxty = 0;

        while (curx >= -400 && cury >= -400) {
            nxtx = curx - tt * tag;
            nxty = cury - tt * (1 - tag);
            tag ^= 1;
            make_path(curx, cury, nxtx, nxty, num_path, 15);
            curx = nxtx, cury = nxty;
        }
        ++ num_path;

        while (curx <= 0 && cury <= 0) {
            nxtx = curx + tt * tag;
            nxty = cury + tt * (1 - tag);
            tag ^= 1;
            make_path(curx, cury, nxtx, nxty, num_path, 15);
            curx = nxtx, cury = nxty;
        }
        //++ num_path;
        //pic_num = num_path;
        if (!car.motion)
            car.motion = motion_flag = 1;
        flag_ = 1;
    }
        **************************/
}

void ips114_displayimage032_zoomT(uint8 *p, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height)
{
    uint32 i, j;
    uint16 color = 0;
    uint16 temp = 0;
    ips114_set_region(0, 0, dis_width - 1, dis_height - 1); //设置显示区域
    for (i = 0; i < dis_height; i++)
        for (int j = 0; j < dis_width; j++)
        {
            temp = *(p + (j * width / dis_width) * height + (dis_height - 1 - i) * height / dis_height);
            color = (0x001f & ((temp) >> 3)) << 11;
            color = color | (((0x003f) & ((temp) >> 2)) << 5);
            color = color | (0x001f & ((temp) >> 3));
            ips114_writedata_16bit(color);
        }
}

void show_red_corner(node LS)
{
    uint16_t x_tmp = LS.x * 235 / MT9V03X_CSI_W, y_tmp = (MT9V03X_CSI_H - LS.y - 1) * 135 / MT9V03X_CSI_H;
    for (int i = x_tmp - 1; i < x_tmp + 2; ++i)
        for (int j = y_tmp - 1; j < y_tmp + 2; ++j)
            ips114_drawpoint(i, j, RED);
}

void show_red_node(int x, int y)
{
    uint16_t x_tmp = x * 239 / 700, y_tmp = 134 - ((y)*134 / 500);
    for (int i = x_tmp - 1; i < x_tmp + 2; ++i)
        for (int j = y_tmp - 1; j < y_tmp + 2; ++j)
            ips114_drawpoint(i, j, RED);
}