#ifndef _MORE_UART_H
#define _MORE_UART_H

#include <headfile.h>

#define SP_OK 1
#define SP_HEADER_ERR -1
#define SP_TAIL_ERR -2
#define SP_WAIT 0
#define PACK_LEN 21

extern uint8 uart8_rx_buffer[2 * PACK_LEN];
extern lpuart_transfer_t   uart8_receivexfer;
extern lpuart_handle_t     uart8_lpuartHandle;
extern char uart_buff[PACK_LEN];
extern float bar_x;

void uart8_init(void);
void uart8_callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData);
void rt_kprintf8(const char *fmt, ...);
void data_analysis(void);

#endif