#include <headfile.h>

void put_img(int width, int height);

int timer_10ms(void);

// **************************** 变量定义 ****************************
int main(void)
{
  rt_thread_mdelay(200);
  ICM_INIT();
  pid_init();
  encoder_init();
  // uart1_init();

  rt_thread_mdelay(1000);
  IPS_INIT(); // need to be put behind of pid_init()
  mt9v03x_csi_init();
  uart8_init();
  motor_init();
  servo_init();
  Wdog_init();
  rt_kprintf("System start!\n");
  EnableGlobalIRQ(0);
  while (1)
  {
    
    
    if ((car.motion == 3 && car.pic_flag == 0) || car.motion == 11)
    {
      static int flag = 0;
      if (!flag)
      {
        rt_thread_mdelay(300), flag = 1;
        rt_sem_take(camera_sem, RT_WAITING_FOREVER);
        car.is_pic = get_pic();
        if (car.is_pic)
        {
          x_pos1 = y_pos1 = 0;
          car.motion = 5;
          car.pic_flag = 1;
          flag = 0;
        }
        else
          car.motion = 11;
      }
      else
      {
        rt_sem_take(camera_sem, RT_WAITING_FOREVER);
        car.is_pic = get_pic();
        if (car.pic_stop)
        {
          rt_thread_mdelay(300);
          rt_sem_take(camera_sem, RT_WAITING_FOREVER);
          car.is_pic = get_pic();
          x_pos1 = y_pos1 = 0;
          car.motion = 5;
          car.pic_flag = 1;
          flag = 0;
          car.pic_stop = 0;
        }
      }
    }
    
    //get_pic();
    rt_thread_mdelay(10);
  }
}

/*********
void put_img(int width, int height)
{

  for(int i = 0; i < height; ++i)
    for(int j = 0; j < width; j++)
     tmp[i][j] = mt9v03x_csi_image[i][j];

  for(int i = 0; i < height; i++)
  {
      for(int j = 0; j < width; ++j)
      rt_kprintf("%d ", tmp[i][j]);
      rt_kprintf("\n");
  }
  rt_kprintf("-------------------------\n-----------------------\n---------------------\n\n\n\n\n\n\n\n");
}
*************/
