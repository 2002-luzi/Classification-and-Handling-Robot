/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2019,?????
 * All rights reserved.
 * ????????QQ???????179029047(????)  ?????244861897
 *
 * ??????????????????????????��?��??????????????????????
 * ?????��??��????????????????????????????????????????
 *
 * @file       		uart
 * @company	   		??????????????
 * @author     		?????(QQ3184284598)
 * @version    		??doc??version??? ?��???
 * @Software 		IAR 8.3 or MDK 5.28
 * @Target core		NXP RT1064DVL6A
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2019-04-30
 ********************************************************************************************************************/
 
 
 #ifndef _ZF_UART_C
 #define _ZF_UART_C

#include "zf_iomuxc.h"
#include "board.h"
#include "zf_uart.h"

LPUART_Type * UARTN[] = LPUART_BASE_PTRS;

#define LPUART_PIN_CONF SPEED_100MHZ | KEEPER_EN | DSE_R0_6 //????UART???????????

void uart_iomuxc(UARTN_enum uartn, UARTPIN_enum tx_pin, UARTPIN_enum rx_pin)
{
    switch(uartn)
    {
        case USART_1:
        {
            if      (UART1_TX_B12 == tx_pin)    iomuxc_pinconf(B12,ALT2,LPUART_PIN_CONF); 
            if      (UART1_RX_B13 == rx_pin)    iomuxc_pinconf(B13,ALT2,LPUART_PIN_CONF); 
        }break;                                                
        case USART_2:                                          
        {                                                      
            if      (UART2_TX_B18 == tx_pin)    iomuxc_pinconf(B18,ALT2,LPUART_PIN_CONF); 
            else if (UART2_TX_D11 == tx_pin)    iomuxc_pinconf(D11,ALT2,LPUART_PIN_CONF); 
                                                               
            if      (UART2_RX_B19 == rx_pin)    iomuxc_pinconf(B19,ALT2,LPUART_PIN_CONF); 
            else if (UART2_RX_D10 == rx_pin)    iomuxc_pinconf(D10,ALT2,LPUART_PIN_CONF); 
        }break;                                                
        case USART_3:                                          
        {                                                      
            if      (UART3_TX_B22 == tx_pin)    iomuxc_pinconf(B22,ALT2,LPUART_PIN_CONF); 
            else if (UART3_TX_C8  == tx_pin)    iomuxc_pinconf(C8 ,ALT3,LPUART_PIN_CONF); 
            else if (UART3_TX_E13 == tx_pin)    iomuxc_pinconf(E13,ALT2,LPUART_PIN_CONF); 
                                                                  
            if      (UART3_RX_B23 == rx_pin)    iomuxc_pinconf(B23,ALT2,LPUART_PIN_CONF); 
            else if (UART3_RX_C9  == rx_pin)    iomuxc_pinconf(C9 ,ALT3,LPUART_PIN_CONF); 
            else if (UART3_RX_E14 == rx_pin)    iomuxc_pinconf(E14,ALT2,LPUART_PIN_CONF); 
        }break;                                                
        case USART_4:                                          
        {                                                      
            if      (UART4_TX_C16 == tx_pin)    iomuxc_pinconf(C16,ALT2,LPUART_PIN_CONF); 
            else if (UART4_TX_D0  == tx_pin)    iomuxc_pinconf(D0 ,ALT4,LPUART_PIN_CONF); 
            else if (UART4_TX_E19 == tx_pin)    iomuxc_pinconf(E19,ALT2,LPUART_PIN_CONF); 
                                                                  
            if      (UART4_RX_C17 == rx_pin)    iomuxc_pinconf(C17,ALT2,LPUART_PIN_CONF); 
            else if (UART4_RX_D1  == rx_pin)    iomuxc_pinconf(D1 ,ALT4,LPUART_PIN_CONF); 
            else if (UART4_RX_E20 == rx_pin)    iomuxc_pinconf(E20,ALT2,LPUART_PIN_CONF); 
        }break;                                                
        case USART_5:                                          
        {                                                      
            if      (UART5_TX_C28 == tx_pin)    iomuxc_pinconf(C28,ALT1,LPUART_PIN_CONF); 
            else if (UART5_TX_E23 == tx_pin)    iomuxc_pinconf(E23,ALT2,LPUART_PIN_CONF); 
                                                                  
            if      (UART5_RX_C29 == rx_pin)    iomuxc_pinconf(C29,ALT1,LPUART_PIN_CONF); 
            else if (UART5_RX_E24 == rx_pin)    iomuxc_pinconf(E24,ALT2,LPUART_PIN_CONF); 
        }break;                                                
        case USART_6:                                          
        {                                                      
            if      (UART6_TX_B2  == tx_pin)    iomuxc_pinconf(B2 ,ALT2,LPUART_PIN_CONF); 
            else if (UART6_TX_E25 == tx_pin)    iomuxc_pinconf(E25,ALT2,LPUART_PIN_CONF); 
                                                                  
            if      (UART6_RX_B3  == rx_pin)    iomuxc_pinconf(B3 ,ALT2,LPUART_PIN_CONF); 
            else if (UART6_RX_E26 == rx_pin)    iomuxc_pinconf(E26,ALT2,LPUART_PIN_CONF); 
        }break;                                                
        case USART_7:                                          
        {                                                      
            if      (UART7_TX_D8  == tx_pin)    iomuxc_pinconf(D8 ,ALT2,LPUART_PIN_CONF); 
            else if (UART7_TX_E31 == tx_pin)    iomuxc_pinconf(E31,ALT2,LPUART_PIN_CONF); 
                                                                  
            if      (UART7_RX_D9  == rx_pin)    iomuxc_pinconf(D9 ,ALT2,LPUART_PIN_CONF); 
            else if (UART7_RX_D18 == rx_pin)    iomuxc_pinconf(D18,ALT2,LPUART_PIN_CONF); 
        }break;                                                
        case USART_8:                                          
        {                                                      
            if      (UART8_TX_B26 == tx_pin)    iomuxc_pinconf(B26,ALT2,LPUART_PIN_CONF); 
            else if (UART8_TX_D16 == tx_pin)    iomuxc_pinconf(D16,ALT2,LPUART_PIN_CONF); 
            else if (UART8_TX_D24 == tx_pin)    iomuxc_pinconf(D24,ALT2,LPUART_PIN_CONF); 
                                                                  
            if      (UART8_RX_B27 == rx_pin)    iomuxc_pinconf(B27,ALT2,LPUART_PIN_CONF); 
            else if (UART8_RX_D17 == rx_pin)    iomuxc_pinconf(D17,ALT2,LPUART_PIN_CONF); 
            else if (UART8_RX_D25 == rx_pin)    iomuxc_pinconf(D25,ALT2,LPUART_PIN_CONF); 
        }break;
        
        default: assert(0);break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ????????
//  @param      uartn           ????????(USART_1,USART_2,USART_3,USART_4,USART_5,USART_6,USART_7,USART_8)
//  @param      baud            ?????????
//  @param      tx_pin          ???????????
//  @param      rx_pin          ???????????
//  @return     uint32          ????????
//  Sample usage:               uart_init(USART_1,115200,UART1_TX_B12,UART1_RX_B13);       // ?????????1 ??????115200 ???????????B12 ???????????B13
//-------------------------------------------------------------------------------------------------------------------
void uart_init(UARTN_enum uartn, uint32 baud, UARTPIN_enum tx_pin, UARTPIN_enum rx_pin)
{
    uint32_t uartClkSrcFreq;
    lpuart_config_t lpuartConfig;
    
    uart_iomuxc(uartn,tx_pin,rx_pin);
    uartClkSrcFreq = BOARD_DebugConsoleSrcFreq();
    LPUART_GetDefaultConfig(&lpuartConfig);
    lpuartConfig.baudRate_Bps = baud;
    lpuartConfig.enableTx = true;
    lpuartConfig.enableRx = true;
    
    LPUART_Init(UARTN[uartn],&lpuartConfig,uartClkSrcFreq); //????��????????????
    LPUART_Deinit(UARTN[uartn]);                            //??��????
    LPUART_Init(UARTN[uartn],&lpuartConfig,uartClkSrcFreq); //????????????????????
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ??????????
//  @param      uartn           ????????(USART_1,USART_2,USART_3,USART_4,USART_5,USART_6,USART_7,USART_8)
//  @param      dat             ???????????
//  @return     void        
//  Sample usage:               uart_putchar(USART_1,0xA5);       // ????1????0xA5
//-------------------------------------------------------------------------------------------------------------------
void uart_putchar(UARTN_enum uartn, uint8 dat)
{
    while (!(UARTN[uartn]->STAT & LPUART_STAT_TDRE_MASK));
    LPUART_WriteByte(UARTN[uartn],dat);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ???????????
//  @param      uartn           ????????(USART_1,USART_2,USART_3,USART_4,USART_5,USART_6,USART_7,USART_8)
//  @param      *buff           ????????????
//  @param      len             ???????
//  @return     void
//  Sample usage:               uart_putbuff(USART_1,&a[0],5);
//-------------------------------------------------------------------------------------------------------------------
void uart_putbuff(UARTN_enum uartn, uint8 *buff, uint32 len)
{
    LPUART_WriteBlocking(UARTN[uartn], buff, len);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ????????????
//  @param      uartn           ????????(USART_1,USART_2,USART_3,USART_4,USART_5,USART_6,USART_7,USART_8)
//  @param      *str            ??????????????
//  @return     void
//  Sample usage:               uart_putstr(USART_1,"i lvoe you"); 
//-------------------------------------------------------------------------------------------------------------------
void uart_putstr(UARTN_enum uartn, const int8 *str)
{
    while(*str)
    {
        uart_putchar(uartn, *str++);
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ????????????????whlie?????
//  @param      uartn           ????????(USART_1,USART_2,USART_3,USART_4,USART_5,USART_6,USART_7,USART_8)
//  @param      *dat            ???????????
//  @return     void        
//  Sample usage:               uint8 dat; uart_getchar(USART_1,&dat);       // ???????1????  ??????dat??????
//-------------------------------------------------------------------------------------------------------------------
void uart_getchar(UARTN_enum uartn, uint8 *dat)
{
    while (!((UARTN[uartn]->WATER & LPUART_WATER_RXCOUNT_MASK) >> LPUART_WATER_RXCOUNT_SHIFT));
    *dat = LPUART_ReadByte(UARTN[uartn]);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ????????????????????????
//  @param      uartn           ????????(USART_1,USART_2,USART_3,USART_4,USART_5,USART_6,USART_7,USART_8)
//  @param      *dat            ???????????
//  @return     uint8           1????????   0??��?????????
//  Sample usage:               uint8 dat; uart_query(USART_1,&dat);       // ???????1????  ??????dat??????
//-------------------------------------------------------------------------------------------------------------------
uint8 uart_query(UARTN_enum uartn, uint8 *dat)
{
    if((UARTN[uartn]->WATER & LPUART_WATER_RXCOUNT_MASK) >> LPUART_WATER_RXCOUNT_SHIFT)
    {
        *dat = LPUART_ReadByte(UARTN[uartn]);
        return 1;
    }
    return 0;
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      ???????????��?????
//  @param      uartn           ????????(USART_1,USART_2,USART_3,USART_4,USART_5,USART_6,USART_7,USART_8)
//  @param      status          1?????��?   0??????��?
//  @return     void        
//  Sample usage:               uart_tx_irq(USART_1,1);       // ?????1????????��?
//-------------------------------------------------------------------------------------------------------------------
void uart_tx_irq(UARTN_enum uartn,uint8 status)
{
    if(status)  
    {
        LPUART_EnableInterrupts(UARTN[uartn], kLPUART_TransmissionCompleteInterruptEnable);
    }
    else
    {
        LPUART_DisableInterrupts(UARTN[uartn], kLPUART_TransmissionCompleteInterruptEnable);
    }
    
    //NVIC_SetPriority((IRQn_Type)((IRQn_Type)(uartn) + LPUART1_IRQn),0);//????????? ??��0-15 ?��????????
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      ????????��?????
//  @param      uartn           ????????(USART_1,USART_2,USART_3,USART_4,USART_5,USART_6,USART_7,USART_8)
//  @param      status          1?????��?   0??????��?
//  @return     void        
//  Sample usage:               uart_rx_irq(USART_1,1);       // ?????1?????��?
//-------------------------------------------------------------------------------------------------------------------
void uart_rx_irq(UARTN_enum uartn,uint8 status)
{
    if(status)  
    {
        LPUART_EnableInterrupts(UARTN[uartn], kLPUART_RxDataRegFullInterruptEnable);
    }
    else
    {
        LPUART_DisableInterrupts(UARTN[uartn], kLPUART_RxDataRegFullInterruptEnable);
    }
    //NVIC_SetPriority((IRQn_Type)((IRQn_Type)(uartn) + LPUART1_IRQn),0);//????????? ??��0-15 ?��????????
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ????????��?????
//  @param      uartn           ????????(USART_1,USART_2,USART_3,USART_4,USART_5,USART_6,USART_7,USART_8)
//  @param      *handle			?????��???????	
//  @param      *callback		?��???????��????
//  @param      *tx_buff		??????????????
//  @param      *tx_count	    ?????????????
//  @param      *rx_buff		??????????????
//  @param      *rx_count		?????????????	
//  @return     void        
//  Sample usage:               uart_set_handle(USART_1, &csi_g_lpuartHandle, uart1_call_back, NULL, 0, buff, 1);//????1 ???��????????????uart1_call_back ??????????��? ????????buff ?????1???????��?
//-------------------------------------------------------------------------------------------------------------------
void uart_set_handle(UARTN_enum uartn, lpuart_handle_t *handle, lpuart_transfer_callback_t callback, uint8 *tx_buff, uint32 tx_count, uint8 *rx_buff, uint32 rx_count)
{
    LPUART_TransferCreateHandle(UARTN[uartn], handle, callback, NULL);
    //?????????????????????
    handle->txData = tx_buff;
    handle->txDataSize = tx_count;
    handle->rxData = rx_buff;
    handle->rxDataSize = rx_count;
    
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ?????????????????????????????????�˧�??
//  @param      uartn           ????????(USART_1,USART_2,USART_3,USART_4,USART_5,USART_6,USART_7,USART_8)
//  @param      float           ???????????          
//  @return     void
//  Sample usage:               uart_putfloat(USART_1, (float)a);
//-------------------------------------------------------------------------------------------------------------------
void uart_putfloat(UARTN_enum uartn, float num)
{

    float _num = fabs(num);
    int tag = num < 0 ? -1 : 1;
    int t = 6, integ = (int)_num, ans = 0, res = 0;
    float sec = _num - integ;
    char text[4];

    while (t --) {
        ans += (int) (sec * 10);
        if (ans == 0) ++ res;
        ans *= 10, sec *= 10;
        sec = sec - (int)sec; 
    }

    ans = ans / pow(10, res ? res - 1 : 1);

    if (tag < 0) uart_putchar(uartn, '-');
	sprintf(text, "%d", integ);
    uart_putstr(uartn, text);
		uart_putchar(uartn, '.');
    while (res --) sprintf(text, "%d", 0), uart_putstr(uartn, text);
		sprintf(text, "%d", ans);
    uart_putstr(uartn, text);
}

#endif