#include "more_uart.h"

uint8 uart8_rx_buffer[2 * PACK_LEN];
lpuart_transfer_t uart8_receivexfer;
lpuart_handle_t uart8_lpuartHandle;
static rt_mailbox_t uart8_mb, data_mb;
char uart_buff[PACK_LEN];
float bar_x;

void uart8_mb_recv(void *parameter);
int uart8_mailbox(void);

void uart8_init(void)
{
    uart_init(USART_8, 115200, UART8_TX_D16, UART8_RX_D17); //串口初始化
    NVIC_SetPriority(LPUART8_IRQn, 0);                     //设置串口中断优先级，0-15，越大优先级越低
    uart_rx_irq(USART_8, 1);                                //打开串口中断

    uart8_receivexfer.dataSize = 2 * PACK_LEN; // get 2 times of the pack length data for data stability
    uart8_receivexfer.data = uart8_rx_buffer;

    uart_set_handle(USART_8, &uart8_lpuartHandle, uart8_callback, NULL, 0, uart8_receivexfer.data, 1); //设置串口中断回调函数及发送接收参数

    uart8_mailbox();
}

//串口1中断回调函数
void uart8_callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
    if (kStatus_LPUART_RxIdle == status)
    {
        rt_mb_send(uart8_mb, uart8_rx_buffer);
    }
    handle->rxDataSize = uart8_receivexfer.dataSize; //还原缓冲区长度
    handle->rxData = uart8_receivexfer.data;         //还原缓冲区地址
}

int uart8_mailbox(void)
{
    rt_thread_t uart8_mb_recv_t, data_t, send_to_art_t;
    uart8_mb = rt_mb_create(
        "uart8",         //邮箱名称
        1,               //缓冲区邮件数
        RT_IPC_FLAG_FIFO //先进先出
    );
    uart8_mb_recv_t = rt_thread_create(
        "uart8_t",     //线程名称
        uart8_mb_recv, //线程入口函数
        RT_NULL,       //线程参数
        4096,           //栈空间大小
        15,            //线程优先级
        20             //时间片
    );
    if (uart8_mb_recv_t != NULL)
        rt_thread_startup(uart8_mb_recv_t);

    data_mb = rt_mb_create("data", 1, RT_IPC_FLAG_FIFO);
    data_t = rt_thread_create("data_t", data_analysis, RT_NULL, 1024, 26, 10);
    if (data_t != NULL)
        rt_thread_startup(data_t);
    return 0;
}

void uart8_mb_recv(void *parameter)
{
    uint32 data_;
    char *data;
    while (1)
    {
        if (rt_mb_recv(
                uart8_mb,           //邮箱控制块
                &data_,             //接受邮箱的字符串 大小32bit
                RT_WAITING_FOREVER) //一直等待
            == RT_EOK)
        {
            data = data_;
            int i = 0;
            static uint32_t last_t = 0;
            while (i <= PACK_LEN)
            {
                if (data[i] == '$' && data[i + PACK_LEN - 1] == '\n')
                    break;
                ++i;
            }
            if (i <= PACK_LEN)
            {
                for (int cnt = 0; cnt < PACK_LEN; ++cnt)
                    uart_buff[cnt] = data[cnt + i];
                car.uart = SP_OK;
                rt_mb_send(data_mb, (uint32)car.uart);
                // rt_kprintf8("time %d\n", rt_tick_get_millisecond() - last_t);
                // last_t = rt_tick_get_millisecond();
            }
            else
                car.uart = SP_HEADER_ERR;
            // for(int i = 0; i < 2 * PACK_LEN; i++)
            //   rt_kprintf("%c",data[i]);
            // rt_kprintf("\n------------------------------------------\n");
        }
    }
}

void rt_hw_console_output8(const char *str)
{
    while (RT_NULL != *str)
    {
        if ('\n' == *str)
        {
            uart_putchar(USART_8, '\r');
        }
        uart_putchar(USART_8, *str++);
    }
}

void rt_kprintf8(const char *fmt, ...)
{
    va_list args;
    rt_size_t length;
    static char rt_log_buf[RT_CONSOLEBUF_SIZE];

    va_start(args, fmt);
    /* the return value of vsnprintf is the number of bytes that would be
     * written to buffer had if the size of the buffer been sufficiently
     * large excluding the terminating null byte. If the output string
     * would be larger than the rt_log_buf, we have to adjust the output
     * length. */
    length = rt_vsnprintf(rt_log_buf, sizeof(rt_log_buf) - 1, fmt, args);
    if (length > RT_CONSOLEBUF_SIZE - 1)
        length = RT_CONSOLEBUF_SIZE - 1;
#ifdef RT_USING_DEVICE
    if (_console_device == RT_NULL)
    {
        rt_hw_console_output(rt_log_buf);
    }
    else
    {
        rt_uint16_t old_flag = _console_device->open_flag;

        _console_device->open_flag |= RT_DEVICE_FLAG_STREAM;
        rt_device_write(_console_device, 0, rt_log_buf, length);
        _console_device->open_flag = old_flag;
    }
#else
    rt_hw_console_output8(rt_log_buf);
#endif
    va_end(args);
}

void data_analysis(void)
{
    /**************
     * e.g:   $&X700.0Y500.0N020.0\n
     *       / ||     |     |      \
     *      /  | \   Y pos  cnt     tail frame
     *     /   |  X pos
     *    /  function frame:get positions of pictures
     * header frame
     *
     *        $*AAAAAAAAAAAAAAAAAA\n
     *         |       \
     *         |     useless frames
     * function:all postions have been transmitted
     *
     *        $@X050.0Y010.0R100.0\n
     *         |            |
     *         |          the angle that the pic rotates
     *         |
     * function: when having gone to a pic, get the pos of this pic to help adjust
     *
     *        $#X001.0AAAAAAAAAAAA\n
     *         | |      |
     *         | \   useless frames
     *        /    the catagory serial number
     * function: get the catagory of the pic
     **************/
    uint32 get;
    int flag;
    while (1)
    {
        if (rt_mb_recv(data_mb, &get, RT_WAITING_FOREVER) == RT_EOK)
        {
            flag = (uint8_t)get;
            if (flag)
            {
                static uint8 pos_cnt = 0;
                if (uart_buff[1] == '&') // get positions of pictures
                {
                    uint8_t n;
                    if (uart_buff[14] == 'N')
                        n = (uart_buff[16] - 48) * 10 + uart_buff[17] - 48;
                    //rt_kprintf("n %d\tpos %d\n", n, pos_cnt);
                    if (n == pos_cnt)
                    {
                        if (uart_buff[2] == 'X') // X position
                            px[pos_cnt] = (uart_buff[3] - 48) * 100 + (uart_buff[4] - 48) * 10 + (uart_buff[5] - 48);
                        if (uart_buff[8] == 'Y') // Y position
                            py[pos_cnt] = (uart_buff[9] - 48) * 100 + (uart_buff[10] - 48) * 10 + (uart_buff[11] - 48);
                        ++pos_cnt;
                    }
                }
                /*if (uart_buff[1] == '*')
                {
                    for (int i = 0; i < pos_cnt; i++)
                        rt_kprintf("X = %d\tY = %d\tN = %d\n", px[i], py[i], i);
                    ant_num = pos_cnt;
                    cur_index = child1_option;
                    rt_mb_send(ips_mb, (rt_ubase_t)KEY_NEXT);
                }*/
                if (uart_buff[1] == '@')
                {
                    if (!car.pic_flag)
                    {
                        if (uart_buff[2] == 'X')
                            pic_x = (uart_buff[3] - 48) * 100 + (uart_buff[4] - 48) * 10 + (uart_buff[5] - 48) + (uart_buff[7] - 48) * 0.1;
                        if (uart_buff[8] == 'Y')
                            pic_y = (uart_buff[9] - 48) * 100 + (uart_buff[10] - 48) * 10 + (uart_buff[11] - 48) + (uart_buff[13] - 48) * 0.1;
                        if (uart_buff[14] == 'L')
                            pic_yaw = ((uart_buff[15] - 48) * 100 + (uart_buff[16] - 48) * 10 + (uart_buff[17] - 48) + (uart_buff[19] - 48) * 0.1);
                        if (uart_buff[14] == 'R')
                            pic_yaw = -((uart_buff[15] - 48) * 100 + (uart_buff[16] - 48) * 10 + (uart_buff[17] - 48) + (uart_buff[19] - 48) * 0.1);
                        // rt_kprintf("X%dY%d\n", (int)pic_x, (int)pic_y);
                        // car.motion = 5;
                    }
                }
                if (uart_buff[1] == '#')
                {
                    /*
                    if (uart_buff[2] == 'A')
                    {
                        clas[path[0][target_cnt]] = (uart_buff[3] - 48) * 100 + (uart_buff[4] - 48) * 10 + (uart_buff[5] - 48);
                        rt_kprintf("class:%d\n", clas[path[0][target_cnt]]);
                        clas[path[0][target_cnt]] = (clas[path[0][target_cnt]] - 1) / 5 + 1;
                        if(clas[path[0][target_cnt]] == 1)
                            clas[path[0][target_cnt]] = 0;
                        if(clas[path[0][target_cnt]] == 3)
                            clas[path[0][target_cnt]] = 1;
                    }
                    */
                    if (uart_buff[2] == 'A')
                    {                        
                        class[lsttt] = (uart_buff[3] - 48) * 100 + (uart_buff[4] - 48) * 10 + (uart_buff[5] - 48);
                        clas[lsttt] = (class[lsttt] - 1) / 5 + 1;
                        if(clas[lsttt] == 1) clas[lsttt] = 0;
                        if(clas[lsttt] == 3) clas[lsttt] = 1;
                    }
                    //pic_y = 1.0 * py[lsttt] + 1;
                    //pic_x = 1.0 * px[lsttt] + 1;
                    //if(target_cnt < 2 * pic_num - 1)
                    if(target_cnt < pic_num)
                        ACO_to_next();
                    if(target_cnt == pic_num)
                        ACO_to_R();
                    switch(clas[lstttt])
                    {
                        case 0:         //animals
                            car.servo_flag = 1;
                            break;
                        case 1:         //trans
                            car.servo_flag = 3;
                            break;
                        case 2:         //fruits
                            car.servo_flag = 2;
                            break;
                        default:
                            break;
                    }
                    //else
                    //   go_home();
                    y = 0, car.to_pic_cnt  = 0;
                    car.uart_flag = 1;
                }
                if(uart_buff[1] == '%')
                {
                    static int flag = 0;
                    if (uart_buff[2] == 'A')
                        bar_x = (uart_buff[3] - 48) * 100 + (uart_buff[4] - 48) * 10 + (uart_buff[5] - 48) + (uart_buff[7] - 48) * 0.1;
                    if (uart_buff[2] == 'B')
                        bar_x = -((uart_buff[3] - 48) * 100 + (uart_buff[4] - 48) * 10 + (uart_buff[5] - 48) + (uart_buff[7] - 48) * 0.1);
                    if(flag == 1)
                        car.bar_flag = 2;
                    if(!flag)
                        car.bar_flag = 1, flag++;
                    
                    //rt_kprintf("!!!!!!!bar_x "), uart_putfloat(USART_1, bar_x),rt_kprintf("\n");
                }
                car.uart = SP_WAIT;
            }
        }
    }
}