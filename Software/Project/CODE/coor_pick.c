#include "coor_pick.h"

typedef struct LINE
{
	float32_t A, B, C;
} line;

#define ElementType uint8_t
typedef struct Node
{
	ElementType data;
	struct Node *next;
} QNode;

typedef struct
{
	QNode *head;
	QNode *tail;
} Queue;

Queue *CreateQueue()
{ // queue init
	Queue *q = (Queue *)rt_malloc(sizeof(Queue));
	q->head = NULL;
	q->tail = NULL;
	return q;
}

void enqueue(Queue *q, ElementType item)
{
	QNode *qNode = (QNode *)rt_malloc(sizeof(QNode));
	qNode->data = item;
	qNode->next = NULL;
	if (q->head == NULL)
	{
		q->head = qNode;
	}
	if (q->tail == NULL)
	{
		q->tail = qNode;
	}
	else
	{
		q->tail->next = qNode;
		q->tail = qNode;
	}
}

int is_empty(Queue *q) { return (q->head == NULL); }

ElementType dequeue(Queue *q)
{
	QNode *temp = q->head;
	ElementType item;
	if (q->head == q->tail)
	{
		q->head = NULL;
		q->tail = NULL;
	}
	else
		q->head = q->head->next;
	item = temp->data, rt_free(temp);
	return item;
}

node get_prox(float32_t lambda, node p1, node p2)
{ // lambda varies in [0, 1]
	//	p1...lam....res....(1 - lam)......p2
	//  o------------o--------------------o
	node res = {-1, -1};
	res.x = (1 - lambda) * p1.x + lambda * p2.x;
	res.y = (1 - lambda) * p1.y + lambda * p2.y;
	return res;
}

line get_line(node p1, node p2)
{ // make a line
	line T;
	T.A = p2.y - p1.y;
	T.B = p1.x - p2.x;
	T.C = -(p1.x * p2.y - p2.x * p1.y);
	return T;
}

node get_cross(node p1, node p2, node p3, node p4)
{
	// 1. 2 in a line, 3. 4 in another line
	line L1, L2;
	L1 = get_line(p1, p2);
	L2 = get_line(p3, p4);

	float32_t a1, b1, a2, b2, c1, c2;
	a1 = L1.A, a2 = L2.A;
	b1 = L1.B, b2 = L2.B;
	c1 = -L1.C, c2 = -L2.C;

	float32_t deno = a1 * b2 - a2 * b1;
	node res = {-1, -1};
	if (fabs(deno) < 1e-3)
		return res;
	res.x = (c1 * b2 - c2 * b1) / deno;
	res.y = (a1 * c2 - a2 * c1) / deno;
	return res;
}

#define X_num (35) // number of grid in x_axis
#define Y_num (25)
#define baby_step (0.001)

#define img_width (188)
#define img_height (120)
#define N (188)
#define M (120)

#define MAX_pieces (45)
#define inf (0x3f3f3f3f)

uint8_t (*img_origin)[M];
int8_t img[N][M];
int nn, m;
int cnt;	 // number of blocks
int tot;	 // number of the points in (^edge)_zone
int pic_num; // number of pictures
int dir[8][2] = {-1, 0, 0, -1, 1, 0, 0, 1, -1, 1, 1, -1, -1, -1, 1, 1};

// 700 * 500 divides into 35 * 25 areas
node rec[MAX_pieces][MAX_pieces], pos[PIC_NUM], PIC[PIC_NUM];
// pos - real position; PIC - picture in img
node LS, LX, RS, RX;	 // in rect
node LSS, LXX, RSS, RXX; // in img
node_int edge[N << 4], pos_int[PIC_NUM];
// smaller --> more accurate
// 0.0001 * 500 = 0.05cm

uint8_t a4_UL[COR_H][COR_W] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0,
	2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 0,
	2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 0,
	2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 0,
	2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 0,
	2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 0,
	2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 0,
	2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 0};

uint8_t a4_UR[COR_H][COR_W] = {
	2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 0,
	2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 0,
	2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 0,
	2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 0,
	2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 0,
	2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 0,
	2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint8_t a4_DR[COR_H][COR_W] = {
	0, 1, 2, 0, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 0, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 0, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 0, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 0, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 0, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 0, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint8_t a4_DL[COR_H][COR_W] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 2, 0, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 0, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 0, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 0, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 0, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 0, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 0, 2, 2, 2, 2, 2, 2, 2};

int cor[4][2];

uint8_t GetOSTU(void);
void get_smlarA4(void);
void line_(int i0, int j0, int i1, int j1);

float32_t diss(node p1, node p2)
{
	return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

typedef struct BLOB
{
	int id, num, sum_x, sum_y;
	float32_t aver_x, aver_y;
} blob;

blob block[PIC_NUM];
// img: 320 * 240  pixels[5, 30]

int is_picture(blob pic)
{ // parameters
	if (pic.num < 1 || pic.num > 25)
		return 0;
	// more to check if its a pic or not
	return 1;
}

void BFS(int x, int y, int cnt, int _4or_8, int tag)
{
	Queue *qx = CreateQueue();
	Queue *qy = CreateQueue();
	enqueue(qx, x), enqueue(qy, y);
	img[x][y] = cnt, ++block[cnt].num;
	block[cnt].sum_x += x, block[cnt].sum_y += y;

	while (!is_empty(qx))
	{
		int nx = dequeue(qx), ny = dequeue(qy);

		for (int i = 0; i < _4or_8; ++i)
		{
			int dx = nx + dir[i][0], dy = ny + dir[i][1];
			if (dx < 0 || dx >= nn || dy < 0 || dy >= m || img[dx][dy] != -1)
				continue;
			if ((img_origin[nx][ny] == 255) ^ (img_origin[dx][dy] == 255))
			{
				if (tag)
					edge[tot].x = dx, edge[tot].y = dy, ++tot;
				continue;
			}

			img[dx][dy] = cnt, ++block[cnt].num;
			block[cnt].sum_x += dx, block[cnt].sum_y += dy;
			enqueue(qx, dx), enqueue(qy, dy);
		}
	}
}

void init()
{
	nn = img_width, m = img_height;
	pic_num = 0, cnt = 0, tot = 0;

	// 4 corners of img_coordinates
	LXX.x = 0, LXX.y = 0;
	LSS.x = 0, LSS.y = m;
	RXX.x = nn, RXX.y = 0;
	RSS.x = nn, RSS.y = m;

	// init
	get_a4();
	memset(img, -1, sizeof img);

	// search from center to edge (only white_in_rect)
	int mid_x = nn >> 1, mid_y = m >> 1;
	for (int i = mid_x - 10; i < mid_x + 10; ++i)
		for (int j = mid_y - 10; j < mid_y + 10; ++j)
			if (img[i][j] == -1 && img_origin[i][j] == 0)
			{
				// make sure there is no picture near the st_point (_8)
				int flag = 1;

				for (int ii = i - 1; ii <= i + 1; ++ii)
					for (int jj = j - 1; jj <= j + 1; ++jj)
						if (img_origin[ii][jj] == 255)
							flag = 0;

				if (flag)
					BFS(i, j, cnt, 4, 1), ++cnt;
			}

	// search black_rect

	// pick 4 cor_points in black_rect
	LS.x = cor[0][0], LS.y = cor[0][1];
	RS.x = cor[1][0], RS.y = cor[1][1];
	LX.x = cor[2][0], LX.y = cor[2][1];
	RX.x = cor[3][0], RX.y = cor[3][1];

	for (int i = 0; i < tot; ++i)
	{ // search in (^edge_zone)
		int st_x = edge[i].x, st_y = edge[i].y;
		if (img[st_x][st_y] == -1 && img_origin[st_x][st_y] == 255)
			BFS(st_x, st_y, cnt, 8, 0), ++cnt;
	}

	for (int i = 0; i < cnt; ++i)
		block[i].aver_x = 1.0 * block[i].sum_x / block[i].num,
		block[i].aver_y = 1.0 * block[i].sum_y / block[i].num;

	// PIC[i] record coordinates of i-th pictures in img
	pic_num = 0;
	for (int i = 1; i < cnt; ++i)
		if (is_picture(block[i]))
		{
			PIC[pic_num].x = block[i].aver_x;
			PIC[pic_num].y = block[i].aver_y;
			++pic_num;
		}

	memset(rec, 0, sizeof rec);
	float32_t step_x = 1.0 / (X_num), step_y = 1.0 / (Y_num);
	float32_t lam_x, lam_y;
	for (int i = 0; i <= X_num; ++i)
	{

		lam_x = 1.0 * i * step_x;
		node p1 = get_prox(lam_x, LX, RX);
		node p2 = get_prox(lam_x, LS, RS);

		for (int j = 0; j <= Y_num; ++j)
		{
			lam_y = 1.0 * j * step_y;
			node p3 = get_prox(lam_y, LX, LS);
			node p4 = get_prox(lam_y, RX, RS);
			rec[i][j] = get_cross(p1, p2, p3, p4);
		}
	}
}

#define rect_widthh (700)
#define rect_heightt (500)

uint8 tas[6][26][21];
int task_num, TASK_TAG = 0;

int readin_task() {
	// for task0
	tas[0][5][6] = 11, tas[0][8][3] = 15, tas[0][14][4] = 3;
	tas[0][23][3] = 6, tas[0][16][8] = 9, tas[0][21][10] = 4;
	tas[0][17][13] = 14, tas[0][12][14] = 1, tas[0][6][16] = 5;
	tas[0][3][12] = 10, tas[0][18][19] = 8, tas[0][22][17] = 13;

	// for task1
	tas[1][5][3] = 15, tas[1][9][5] = 2, tas[1][15][6] = 8;
	tas[1][20][3] = 13, tas[1][18][9] = 12, tas[1][23][10] = 6;
	tas[1][21][15] = 3, tas[1][14][16] = 10, tas[1][8][18] = 11;
	tas[1][3][17] = 5, tas[1][10][12] = 1, tas[1][4][9] = 7;

	// for task2
	tas[2][3][16] = 1, tas[2][8][17] = 13, tas[2][14][15] = 10;
	tas[2][21][14] = 3, tas[2][23][18] = 12, tas[2][4][12] = 7;
	tas[2][9][11] = 5, tas[2][22][8] = 6, tas[2][18][4] = 2;
	tas[2][14][6] = 15, tas[2][10][3] = 8, tas[2][5][7] = 11;

	// for task3
    tas[3][5][6] = 7, tas[3][9][4] = 12, tas[3][15][3] = 6;
    tas[3][23][7] = 10, tas[3][16][8] = 4, tas[3][11][9] = 1;
    tas[3][3][12] = 14, tas[3][6][17] = 5, tas[3][10][14] = 15;
    tas[3][17][15] = 9, tas[3][22][18] = 2, tas[3][21][11] = 11;

	// for task4
    tas[4][3][5] = 2, tas[4][10][4] = 5, tas[4][16][3] = 6;
    tas[4][22][3] = 3, tas[4][21][8] = 8, tas[4][23][14] = 12;
    tas[4][18][18] = 1, tas[4][11][16] = 13, tas[4][5][15] = 10;
    tas[4][9][12] = 7, tas[4][4][9] = 15, tas[4][14][7] = 11;

	// for task5
    tas[5][5][3] = 5, tas[5][12][4] = 7, tas[5][20][5] = 13;
    tas[5][18][9] = 3, tas[5][22][12] = 10, tas[5][16][13] = 12;
    tas[5][15][17] = 8, tas[5][21][18] = 2, tas[5][9][15] = 1;
    tas[5][3][14] = 11, tas[5][5][10] = 6, tas[5][10][8] = 15;
}

int check_task() {
	//readin_task();
	return -1; 
	
	for (int tasks = 0; tasks < 6; ++ tasks) {
		int cnt = 0;
		for (int i = 0; i < pic_num; ++ i)
			cnt += tas[tasks][pos_int[i].x][pos_int[i].y] > 0;
		if (cnt >= 6) return tasks;	
	} 
	return -1;
}

void find_pic(void)
{
	uint32 a = rt_tick_get_millisecond();

	init();

	for (int id = 0; id < pic_num; ++id)
	{
		// for id-th picture
		float32_t px = PIC[id].x, py = PIC[id].y, minn = 1.0 * inf;

		int tmpi = -1, tmpj = -1;
		for (int i = 1; i < X_num; ++i)
		{
			for (int j = 1; j < Y_num; ++j)
			{
				float32_t DIS = diss(PIC[id], rec[i][j]);
				if (minn > DIS)
					minn = DIS, tmpi = i, tmpj = j;
			}
		}

		// already have tmpi, tmpj (approximately)
		float32_t ansx = 0, ansy = 0;
		minn = 1.0 * inf;
		for (float32_t lam_x = 1.0 * (tmpi - 1) / X_num; lam_x <= 1.0 * (tmpi + 1) / X_num; lam_x += baby_step)
		{

			node p1 = get_prox(lam_x, LX, RX);
			node p2 = get_prox(lam_x, LS, RS);

			for (float32_t lam_y = 1.0 * (tmpj - 1) / Y_num; lam_y <= 1.0 * (tmpj + 1) / Y_num; lam_y += baby_step)
			{

				node p3 = get_prox(lam_y, LX, LS);
				node p4 = get_prox(lam_y, RX, RS);
				node s = get_cross(p1, p2, p3, p4);

				float32_t dis = (s.x - px) * (s.x - px) + (s.y - py) * (s.y - py);
				if (dis < minn)
					minn = dis, ansx = lam_x, ansy = lam_y;
			}
		}
		pos[id].x = ansx * rect_widthh, pos[id].y = ansy * rect_heightt;
	}
	// rt_kprintf("time %d\n", rt_tick_get_millisecond() - a);

	// printf("Number of pictures: %d\n", pic_num);
	for (int i = 0; i < pic_num; ++i)
	{
		pos_int[i].x = (int)(pos[i].x) / 20 + 1;
		pos_int[i].y = (int)(pos[i].y) / 20 + 1;
	}

	task_num = check_task();
	
	// already change the clas[]
	if (task_num != -1) { 
		pic_num = 0;
		for (int i = 1; i <= 25; ++ i)
			for (int j = 1; j <= 20; ++ j)
				if(tas[task_num][i][j]) {
					pos_int[pic_num].x = i, pos_int[pic_num].y = j;
					class[pic_num] = tas[task_num][i][j];
					++ pic_num;
				}
		TASK_TAG = 1;
	}
	
}

/**
#0-th: (627.86, 131.10) --> (32, 7)
#1-th: (567.02, 210.45) --> (29, 11)
#2-th: (547.02, 309.95) --> (28, 16)
#3-th: (506.74, 370.20) --> (26, 19)
#4-th: (628.00, 269.90) --> (32, 14)
#5-th: (647.79, 370.80) --> (33, 19)
#6-th: (586.18, 430.30) --> (30, 22)
#7-th: (627.51, 470.55) --> (32, 24)
 ***/

uint8_t GetOSTU(void)
{
	signed short i, j;
	unsigned long Amount = 0;
	unsigned long PixelBack = 0;
	unsigned long PixelshortegralBack = 0;
	unsigned long Pixelshortegral = 0;
	signed long PixelshortegralFore = 0;
	signed long PixelFore = 0;
	float OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差;
	signed short MinValue, MaxValue;
	uint8_t Threshold = 0;
	unsigned char HistoGram[256]; //

	memset(HistoGram, 0, sizeof HistoGram);

	for (i = 0; i < MT9V03X_CSI_H; i++)
	{
		for (j = 0; j < MT9V03X_CSI_W; j++)
		{
			HistoGram[img_origin[j][i]]++; //统计灰度级中每个像素在整幅图像中的个数
		}
	}

	for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++)
		; //获取最小灰度的值
	for (MaxValue = 255; MaxValue > MinValue && HistoGram[MinValue] == 0; MaxValue--)
		; //获取最大灰度的值

	if (MaxValue == MinValue)
		return MaxValue; // 图像中只有一个颜色
	if (MinValue + 1 == MaxValue)
		return MinValue; // 图像中只有二个颜色

	for (j = MinValue; j <= MaxValue; j++)
		Amount += HistoGram[j]; //  像素总数

	Pixelshortegral = 0;
	for (j = MinValue; j <= MaxValue; j++)
	{
		Pixelshortegral += HistoGram[j] * j; //灰度值总数
	}
	SigmaB = -1;
	for (j = MinValue; j < MaxValue; j++)
	{
		PixelBack = PixelBack + HistoGram[j];											   //前景像素点数
		PixelFore = Amount - PixelBack;													   //背景像素点数
		OmegaBack = (float)PixelBack / Amount;											   //前景像素百分比
		OmegaFore = (float)PixelFore / Amount;											   //背景像素百分比
		PixelshortegralBack += HistoGram[j] * j;										   //前景灰度值
		PixelshortegralFore = Pixelshortegral - PixelshortegralBack;					   //背景灰度值
		MicroBack = (float)PixelshortegralBack / PixelBack;								   //前景灰度百分比
		MicroFore = (float)PixelshortegralFore / PixelFore;								   //背景灰度百分比
		Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore); //计算类间方差
		if (Sigma > SigmaB)																   //遍历最大的类间方差g //找出最大类间方差以及对应的阈值
		{
			SigmaB = Sigma;
			Threshold = j;
		}
	}
	return Threshold; //返回最佳阈值;
}
int check_rct(int lsx, int lsy, int rsx, int rsy, int rxx, int rxy, int lxx, int lxy) {
	
	int wid_ = abs(rsx - lsx) + abs(rxx - lxx) >> 1;
	int wei_ = abs(lsy - lxy) + abs(rxy - rsy) >> 1;
	float dd = 1.0 * wid_ / wei_;
	if (dd < 1.3 || dd > 1.5) return 1;

	int ss = wid_ * wei_;
	if (ss < 4000 || ss > 10000)	return 1; 
	return 0;
}

void get_a4(void)
{
	while (1)
	{
		rt_sem_take(camera_sem, RT_WAITING_FOREVER);
		for (int i = 0; i < MT9V03X_CSI_W; ++i)
			for (int j = 0; j < MT9V03X_CSI_H; ++j)
				img_origin[i][j] = mt9v03x_csi_image[MT9V03X_CSI_H - 1 - j][i];
		int thre = GetOSTU();
		for (int i = 0; i < MT9V03X_CSI_W; ++i)
			for (int j = 0; j < MT9V03X_CSI_H; ++j)
				img_origin[i][j] = (img_origin[i][j] < thre) ? 1 : 0;
    	//ips114_displayimage032_zoom(img_origin, MT9V03X_CSI_W, MT9V03X_CSI_H, 240, 135);
		get_smlarA4();

		int lsx = cor[0][0], lsy = cor[0][1];
		int rsx = cor[1][0], rsy = cor[1][1];
		int rxx = cor[3][0], rxy = cor[3][1];
		int lxx = cor[2][0], lxy = cor[2][1];

		line_(cor[0][0], cor[0][1], cor[1][0], cor[1][1]);
		line_(cor[0][0], cor[0][1], cor[2][0], cor[2][1]);
		line_(cor[2][0], cor[2][1], cor[3][0], cor[3][1]);
		line_(cor[3][0], cor[3][1], cor[1][0], cor[1][1]);

		for (int i = 0; i < MT9V03X_CSI_W; ++i)
			for (int j = 0; j < MT9V03X_CSI_H; ++j)
				img_origin[i][j] *= 255;

		ips114_displayimage032_zoomT(img_origin, MT9V03X_CSI_W, MT9V03X_CSI_H, 240, 135);

		for (int i = 0; i < 4; ++i)
			show_red_corner1(cor[i][0], cor[i][1]);
		int err[4];
		err[0] = abs(cor[0][1] - cor[1][1]);
		err[1] = abs(cor[0][0] - cor[2][0]);
		err[2] = abs(cor[2][1] - cor[3][1]);
		err[3] = abs(cor[3][0] - cor[1][0]);

		//for(int i = 0; i < 4; i++)
		//	rt_kprintf("(%d, %d)\t", cor[i][0], cor[i][1]);
		//rt_kprintf("\n");

		if (err[0] < 6 && err[1] < 6 && err[2] < 6 && err[3] < 6)
			if (!check_rct(lsx, lsy, rsx, rsy, rxx, rxy, lxx, lxy)) 
				break;
		
		rt_thread_delay(50);
	}
}

void get_smlarA4(void)
{
	float smlar_UL, smlar_UR, smlar_DL, smlar_DR;
	smlar_UL = smlar_UR = smlar_DL = smlar_DR = 100;

	memset(cor, 0, sizeof cor);

	/***************UL******************/
	for (int i = 0; i < MT9V03X_CSI_W / 2; ++i)
		for (int j = MT9V03X_CSI_H / 2; j < MT9V03X_CSI_H; ++j)
		{
			int cnt1 = 0;
			int sum = 0;

			for (int k = 0; k < COR_H; ++k)
				for (int l = 0; l < COR_W; ++l)
				{
					if (a4_UL[k][l] != 2)
						sum += abs(img_origin[i + k][j + l] - a4_UL[k][l]);
					if (sum > 4)
						break;
					cnt1++;
				}

			if (cnt1 == COR_H * COR_W)
			{
				if (smlar_UL > sum)
				{
					smlar_UL = sum;
					cor[0][0] = i, cor[0][1] = j;
				}
			}
		}
	/*****************DR********************/
	for (int i = MT9V03X_CSI_W / 2; i < MT9V03X_CSI_W; ++i)
		for (int j = 0; j < MT9V03X_CSI_H / 2; ++j)
		{
			int cnt1 = 0;
			int sum = 0;

			sum = cnt1 = 0;
			for (int k = 0; k < COR_H; ++k)
				for (int l = 0; l < COR_W; ++l)
				{
					if (a4_DR[k][l] != 2)
						sum += abs(img_origin[i + k][j + l] - a4_DR[k][l]);
					if (sum > 4)
						break;
					cnt1++;
				}
			if (cnt1 == COR_H * COR_W)
				if (smlar_DR > sum)
				{
					smlar_DR = sum;
					cor[3][0] = i, cor[3][1] = j;
				}
		}
	/*****************UR********************/
	for (int i = MT9V03X_CSI_W / 2; i < MT9V03X_CSI_W; ++i)
		for (int j = MT9V03X_CSI_H / 2; j < MT9V03X_CSI_H; ++j)
		{
			int cnt1 = 0, sum = 0;

			sum = cnt1 = 0;
			for (int k = 0; k < 11; ++k)
				for (int l = 0; l < 11; ++l)
				{
					if (a4_UR[k][l] != 2)
						sum += abs(img_origin[i + k][j + l] - a4_UR[k][l]);
					if (sum > 4)
						break;
					cnt1++;
				}
			if (cnt1 == COR_H * COR_W)
				if (smlar_UR > sum)
				{
					smlar_UR = sum;
					cor[1][0] = i, cor[1][1] = j;
				}
		}

	/*****************DL********************/
	for (int i = 0; i < MT9V03X_CSI_W / 2; ++i)
		for (int j = 0; j < MT9V03X_CSI_H / 2; ++j)
		{
			int cnt1 = 0, sum = 0;

			sum = cnt1 = 0;
			for (int k = 0; k < COR_H; ++k)
				for (int l = 0; l < COR_W; ++l)
				{
					if (a4_DL[k][l] != 2)
						sum += abs(img_origin[i + k][j + l] - a4_DL[k][l]);
					if (sum > 4)
						break;
					cnt1++;
				}
			if (cnt1 == COR_H * COR_W)
				if (smlar_DL > sum)
				{
					smlar_DL = sum;
					cor[2][0] = i, cor[2][1] = j;
				}
		}

	// rectangle(cor[0][1] + H, cor[0][0], cor[0][1] + H + 8, cor[0][0] + COR_W);
	cor[0][0] += 1, cor[0][1] += 9;
	cor[1][0] += 9, cor[1][1] += 9;
	cor[3][0] += 9, cor[3][1] += 1;
	cor[2][0] += 1, cor[2][1] += 1;
}

void line_(int i0, int j0, int i1, int j1)
{
	int dx = abs(i1 - i0), sx = i0 < i1 ? 1 : -1, dy = -abs(j1 - j0), sy = j0 < j1 ? 1 : -1, err = dx + dy;
	for (;;)
	{
		// plot(i0, j0);
		// img[i0][j0] = 255;
		img_origin[i0][j0] = 1;
		if (i0 == i1 && j0 == j1)
			break;
		int_fast32_t e2 = 2 * err;
		if (e2 >= dy)
			err += dy, i0 += sx;
		if (e2 <= dx)
			err += dx, j0 += sy;
	}
}
