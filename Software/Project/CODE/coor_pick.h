#ifndef _COOR_PICK_H
#define _COOR_PICK_H

#include "headfile.h"

#define PIC_NUM (25)
#define rect_width (700)
#define rect_height (500)

typedef struct { uint8 x, y; } node_int;
typedef struct NODE { float32_t x, y; } node;

void find_pic(void);
void sobel(void);
void get_a4(void);

extern int pic_num;
extern uint8_t (*img_origin)[MT9V03X_CSI_H];
extern node_int pos_int[PIC_NUM];
extern node LS, RS, LX, RX;
extern int gray_thre;
extern int8_t img[MT9V03X_CSI_W][MT9V03X_CSI_H];
extern int task_num, TASK_TAG;

#endif
