#include "pic_pick.h"

uint8_t cor_UL[COR_H][COR_W] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0};

uint8_t cor_UR[COR_H][COR_W] = {
	0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint8_t cor_DR[COR_H][COR_W] = {
	0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint8_t cor_DL[COR_H][COR_W] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0};

//uint8_t st4[2][2][2][2] = {0, 0, 0, 1, 0, 2, 3, 4, 0, 5, 6, 7, 8, 9, 10, 11};

int coor[4][2];
int pic_i, pic_j;

#define THRE 145

void dialation(void)
{
	for (int i = 0; i < MT9V03X_CSI_W - 1; ++i)
		for (int j = 0; j < MT9V03X_CSI_H - 1; ++j)
		{
			if (img[i][j])
			{
				for (int k = 0; k <= 1; ++k)
					for (int ii = 0; ii <= 1; ++ii)
						img_origin[i + k][j + ii] = 1;
			}
		}
}

void erosion(void)
{
	for (int i = 0; i < MT9V03X_CSI_W - 1; ++i)
		for (int j = 0; j < MT9V03X_CSI_H - 1; ++j)
		{
			int cnt = 0;
			for (int k = 0; k <= 1; ++k)
				for (int ii = 0; ii <= 1; ++ii)
					if (img_origin[i + k][j + ii])
						cnt++;
			if (cnt == 4)
				img[i][j] = 1;
		}
}

void sobel(void)
{
	/*for (int i = 1; i < MT9V03X_CSI_H - 1; ++i)
	{
		for (int j = 1; j < MT9V03X_CSI_W - 1; ++j)
		{
			int temp1 = (mt9v03x_csi_image[i + 1][j - 1] + 2 * mt9v03x_csi_image[i + 1][j] + mt9v03x_csi_image[i + 1][j + 1]) - (mt9v03x_csi_image[i - 1][j - 1] + 2 * mt9v03x_csi_image[i - 1][j] + mt9v03x_csi_image[i - 1][j + 1]);

			int temp2 = (mt9v03x_csi_image[i - 1][j - 1] + 2 * mt9v03x_csi_image[i][j - 1] + mt9v03x_csi_image[i + 1][j - 1]) - (mt9v03x_csi_image[i - 1][j + 1] + 2 * mt9v03x_csi_image[i][j + 1] + mt9v03x_csi_image[i + 1][j + 1]);

			if (fabs(temp1) + fabs(temp2) > 89)
				img[j][MT9V03X_CSI_H - 1 - i] = 1;
			else
				img[j][MT9V03X_CSI_H - 1 - i] = 0;
		}
	}*/
	for (int i = 1; i < MT9V03X_CSI_W - 1; ++i)
	{
		for (int j = 1; j < MT9V03X_CSI_H - 1; ++j)
		{
			int temp1 = (img_origin[i + 1][j - 1] + 2 * img_origin[i + 1][j] + img_origin[i + 1][j + 1]) - (img_origin[i - 1][j - 1] + 2 * img_origin[i - 1][j] + img_origin[i - 1][j + 1]);

			int temp2 = (img_origin[i - 1][j - 1] + 2 * img_origin[i][j - 1] + img_origin[i + 1][j - 1]) - (img_origin[i - 1][j + 1] + 2 * img_origin[i][j + 1] + img_origin[i + 1][j + 1]);

			if (fabs(temp1) + fabs(temp2) > 89)
				//img[j][MT9V03X_CSI_H - 1 - i] = 1;
				img[i][j] = 1;
			else
				img[i][j] = 0;
				//img[j][MT9V03X_CSI_H - 1 - i] = 0;
		}
	}
}

void get_similarity(void)
{
	int smlar_UL, smlar_UR, smlar_DL, smlar_DR;
	smlar_UL = smlar_UR = smlar_DL = smlar_DR = 100;

	int min_i = 0, min_j = 0, max_i = 0, max_j = 0, flag_ = 0;
	for (int i = 0; i < MT9V03X_CSI_W; ++i)
		for (int j = 0; j < MT9V03X_CSI_H; ++j)
		{
			if (img[i][j] && !flag_)
				min_i = i, min_j = j, flag_ = 1;
			if (img[i][j])
			{
				if (max_i < i)
					max_i = i;
				if (max_j < j)
					max_j = j;
				if (min_i > i)
					min_i = i;
				if (min_j > j)
					min_j = j;
			}
		}

	if (min_i - COR_H > 0)
		min_i -= COR_H;
	else
		min_i = 0;
	if (min_j - COR_W > 0)
		min_j -= COR_W;
	else
		min_j = 0;
	if (max_i + COR_H < MT9V03X_CSI_W)
		max_i += COR_H;
	else
		max_i = MT9V03X_CSI_W - COR_H;
	if (max_j + COR_W < MT9V03X_CSI_H)
		max_j += COR_W;
	else
		max_j = MT9V03X_CSI_H - COR_W;

	for (int i = min_i; i < max_i; ++i)
		for (int j = min_j; j < max_j; ++j)
		{
			int cnt1 = 0;
			int sum = 0, norm1 = 0, norm2 = 0;

			/***************UL******************/
			for (int k = 0; k < COR_H; ++k)
				for (int l = 0; l < COR_W; ++l)
				{
					sum += abs(img[i + k][j + l] - cor_UL[k][l]);
					if (sum > 10)
						break;
					cnt1++;
				}

			if (cnt1 == COR_H * COR_W)
			{
				if (smlar_UL > sum) // && coor[0][0] >= i && coor[0][1] <= j)
				{
					smlar_UL = sum;
					coor[0][0] = i, coor[0][1] = j;
				}
			}

			/*****************UR********************/
			sum = cnt1 = 0;
			for (int k = 0; k < COR_H; ++k)
				for (int l = 0; l < COR_W; ++l)
				{
					sum += abs(img[i + k][j + l] - cor_UR[k][l]);
					if (sum > 10)
						break;
					cnt1++;
				}
			if (cnt1 == COR_H * COR_W)
				if (smlar_UR > sum)
				{
					smlar_UR = sum;
					coor[1][0] = i, coor[1][1] = j;
				}

			/****************DR********************/
			sum = cnt1 = 0;
			for (int k = 0; k < COR_H; ++k)
				for (int l = 0; l < COR_W; ++l)
				{
					sum += abs(img[i + k][j + l] - cor_DR[k][l]);
					if (sum > 10)
						break;
					cnt1++;
				}
			if (cnt1 == COR_H * COR_W)
				if (smlar_DR > sum)
				{
					smlar_DR = sum;
					coor[3][0] = i, coor[3][1] = j;
				}

			/*****************DL*******************/
			sum = cnt1 = 0;
			for (int k = 0; k < COR_H; ++k)
				for (int l = 0; l < COR_W; ++l)
				{
					sum += abs(img[i + k][j + l] - cor_DL[k][l]);
					if (sum > 10)
						break;
					cnt1++;
				}
			if (cnt1 == COR_H * COR_W)
				if (smlar_DL > sum)
				{
					smlar_DL = sum;
					coor[2][0] = i, coor[2][1] = j;
				}
		}
}

int get_center(void)
{
	int st_UL = 0, st_UR = 0, st_DL = 0, st_DR = 0;

	if (coor[0][0] || coor[0][1])
		st_UL = 1;
	if (coor[1][0] || coor[1][1])
		st_UR = 1;
	if (coor[2][0] || coor[2][1])
		st_DL = 1;
	if (coor[3][0] || coor[3][1])
		st_DR = 1;

	int st = (st_UL << 3) + (st_UR << 2) + (st_DL << 1) + st_DR;
	//rt_kprintf("%d%d%d%d ",st_UL, st_UR, st_DL, st_DR);
	//rt_kprintf("%d\n", st);

	for (int i = 0; i < 4; i++)
		// rt_kprintf("%d %d\n", coor[i][0], coor[i][1]),
		coor[i][0] += 5, coor[i][1] += 5;
	coor[1][0] += 4, coor[3][0] += 4;

	int dis1, dis2, dis3, dis4;
	int st_1, st_2, st_3, st_4;
	int maxx = 53, minn = 44;
	switch(st)
	{
		case 3:
			dis3 = calc_dis(coor[2][0], coor[2][1], coor[3][0], coor[3][1]);
			if(dis3 > maxx || dis3 < minn)
				return 0;
			break;
		case 5:
			dis3 = calc_dis(coor[1][0], coor[1][1], coor[3][0], coor[3][1]);
			if(dis3 > maxx || dis3 < minn)
				return 0;
			break;
		case 6:
			dis3 = calc_dis(coor[1][0], coor[1][1], coor[2][0], coor[2][1]);
			if(dis3 > maxx * 1.414 || dis3 < minn * 1.414)
				return 0;
			break;
		case 7:
			dis2 = calc_dis(coor[1][0], coor[1][1], coor[3][0], coor[3][1]);
			dis3 = calc_dis(coor[2][0], coor[2][1], coor[3][0], coor[3][1]);
			st_2 = (dis2 <= maxx && dis2 >= minn), st_3 = (dis3 <= maxx && dis3 >= minn);
			//rt_kprintf("d2 %d d3 %d\n", dis2, dis3);
			//rt_kprintf("st2 %d st3 %d\n", st_2, st_3);
			if(st_2 && st_3)
				break;
			else if(st_2)
				st_DL = 0;
			else if(st_3)
				st_UR = 0;
			else	return 0;
			break;
		case 9:
			dis3 = calc_dis(coor[0][0], coor[0][1], coor[3][0], coor[3][1]);
			if(dis3 > maxx * 1.414 || dis3 < minn * 1.414)
				return 0;
			break;
		case 10:
			dis3 = calc_dis(coor[0][0], coor[0][1], coor[2][0], coor[2][1]);
			if(dis3 > maxx || dis3 < minn)
				return 0;
			break;
		case 11:
			dis4 = calc_dis(coor[0][0], coor[0][1], coor[2][0], coor[2][1]);
			dis3 = calc_dis(coor[2][0], coor[2][1], coor[3][0], coor[3][1]);
			st_4 = (dis4 <= maxx && dis4 >= minn), st_3 = (dis3 <= maxx && dis3 >= minn);
			if(st_4 && st_3)
				break;
			else if(st_4)
				st_DR = 0;
			else if(st_3)
				st_UL = 0;
			else	return 0;
			break;
		case 12:
			dis3 = calc_dis(coor[0][0], coor[0][1], coor[1][0], coor[1][1]);
			//rt_kprintf("dis = %d\n", dis3);
			if(dis3 > maxx || dis3 < minn)
				return 0;
			break;
		case 13:
			dis1 = calc_dis(coor[0][0], coor[0][1], coor[1][0], coor[1][1]);
			dis2 = calc_dis(coor[1][0], coor[1][1], coor[3][0], coor[3][1]);
			st_1 = (dis1 <= maxx && dis1 >= minn), st_2 = (dis2 <= maxx && dis2 >= minn);
			if(st_1 && st_2) 
				break;
			else if(st_1)
				st_DR = 0;
			else if(st_2)
				st_UL = 0;
			else	return 0;
			break;
		case 14:
			dis1 = calc_dis(coor[0][0], coor[0][1], coor[1][0], coor[1][1]);
			dis4 = calc_dis(coor[0][0], coor[0][1], coor[2][0], coor[2][1]);
			st_4 = (dis4 <= maxx && dis4 >= minn), st_1 = (dis1 <= maxx && dis1 >= minn);
			if(st_4 && st_1)
				break;
			else if(st_4)
				st_UR = 0;
			else if(st_1)
				st_DL = 0;
			else	return 0;
			break;
		case 15:
			dis1 = calc_dis(coor[0][0], coor[0][1], coor[1][0], coor[1][1]);
			dis2 = calc_dis(coor[1][0], coor[1][1], coor[3][0], coor[3][1]);
			dis3 = calc_dis(coor[2][0], coor[2][1], coor[3][0], coor[3][1]);
			dis4 = calc_dis(coor[0][0], coor[0][1], coor[2][0], coor[2][1]);
			st_1 = (dis1 <= maxx && dis1 >= minn), st_2 = (dis2 <= maxx && dis2 >= minn);
			st_3 = (dis3 <= maxx && dis3 >= minn), st_4 = (dis4 <= maxx && dis4 >= minn);
			//rt_kprintf("d1 %d d2 %d d3 %d d4 %d\n", dis1, dis2, dis3, dis3);
			//rt_kprintf("st1 %d st2 %d st3 %d st4 %d\n", st_1, st_2 , st_3, st_4);
			if(st_1 == 0 && st_2 == 1 && st_3 == 1 && st_4 == 1)
				st_UL = st_UR = 0;
			if(st_1 == 1 && st_2 == 0 && st_3 == 1 && st_4 == 1)
				st_DR = st_UR = 0;
			if(st_1 == 1 && st_2 == 1 && st_3 == 0 && st_4 == 1)
				st_DL = st_DR = 0;
			if(st_1 == 1 && st_2 == 1 && st_3 == 1 && st_4 == 0)
				st_UL = st_DL = 0;
			if(st_1 == 0 && st_2 == 0 && st_3 == 1 && st_4 == 1)
				st_UR = 0;
			if(st_1 == 1 && st_2 == 0 && st_3 == 0 && st_4 == 1)
				st_DR = 0;
			if(st_1 == 1 && st_2 == 1 && st_3 == 0 && st_4 == 0)
				st_DL = 0;
			if(st_1 == 0 && st_2 == 1 && st_3 == 1 && st_4 == 0)
				st_UL = 0;
			if(st_1 == 0 && st_2 == 0 && st_3 == 0 && st_4 == 1)
				st_UR = st_DR = 0;
			if(st_1 == 1 && st_2 == 0 && st_3 == 0 && st_4 == 0)
				st_DL = st_DR = 0;
			if(st_1 == 0 && st_2 == 1 && st_3 == 0 && st_4 == 0)
				st_DL = st_UL = 0;
			if(st_1 == 0 && st_2 == 0 && st_3 == 1 && st_4 == 0)
				st_UL = st_UR = 0;
			break;
		default:
			break;
	}

	if (st_UL == 0 && st_UR == 0 && st_DL == 0 && st_DR == 0)
	{
		// rt_kprintf("Cannot find a pic!\n");
		return 0;
	}

	if (st_UL == 1 && st_UR == 0 && st_DL == 0 && st_DR == 0)
		pic_i = coor[0][0] + 30, pic_j = coor[0][1] - 30;
	if (st_UL == 0 && st_UR == 1 && st_DL == 0 && st_DR == 0)
		pic_i = coor[1][0] - 30, pic_j = coor[1][1] - 30;
	if (st_UL == 0 && st_UR == 0 && st_DL == 1 && st_DR == 0)
		pic_i = coor[2][0] + 30, pic_j = coor[2][1] + 30;
	if (st_UL == 0 && st_UR == 0 && st_DL == 0 && st_DR == 1)
		pic_i = coor[3][0] - 30, pic_j = coor[3][1] + 30;

	if (st_UL == 1 && st_UR == 1 && st_DL == 0 && st_DR == 0)
		pic_i = (coor[0][0] + coor[1][0]) / 2, pic_j = (coor[0][1] + coor[1][1]) / 2 - 30;
	if (st_UL == 1 && st_UR == 0 && st_DL == 1 && st_DR == 0)
		pic_i = (coor[0][0] + coor[2][0]) / 2 + 30, pic_j = (coor[0][1] + coor[2][1]) / 2;
	if (st_UL == 0 && st_UR == 0 && st_DL == 1 && st_DR == 1)
		pic_i = (coor[2][0] + coor[3][0]) / 2, pic_j = (coor[2][1] + coor[3][1]) / 2 + 30;
	if (st_UL == 0 && st_UR == 1 && st_DL == 0 && st_DR == 1)
		pic_i = (coor[1][0] + coor[3][0]) / 2 - 30, pic_j = (coor[1][1] + coor[3][1]) / 2;
	if (st_UL == 1 && st_UR == 0 && st_DL == 0 && st_DR == 1)
		pic_i = (coor[0][0] + coor[3][0]) / 2, pic_j = (coor[0][1] + coor[3][1]) / 2;
	if (st_UL == 0 && st_UR == 1 && st_DL == 1 && st_DR == 0)
		pic_i = (coor[1][0] + coor[2][0]) / 2, pic_j = (coor[1][1] + coor[2][1]) / 2;

	if (st_UL == 1 && st_UR == 1 && st_DL == 1 && st_DR == 0)
		pic_i = (coor[2][0] + coor[1][0]) / 2, pic_j = (coor[2][1] + coor[1][1]) / 2;
	if (st_UL == 1 && st_UR == 1 && st_DL == 0 && st_DR == 1)
		pic_i = (coor[0][0] + coor[3][0]) / 2, pic_j = (coor[0][1] + coor[3][1]) / 2;
	if (st_UL == 0 && st_UR == 1 && st_DL == 1 && st_DR == 1)
		pic_i = (coor[1][0] + coor[2][0]) / 2, pic_j = (coor[1][1] + coor[2][1]) / 2;
	if (st_UL == 1 && st_UR == 0 && st_DL == 1 && st_DR == 1)
		pic_i = (coor[0][0] + coor[3][0]) / 2, pic_j = (coor[0][1] + coor[3][1]) / 2;

	if (st_UL == 1 && st_UR == 1 && st_DL == 1 && st_DR == 1)
		pic_i = (coor[1][0] + coor[2][0] + coor[3][0] + coor[0][0]) / 4, pic_j = (coor[0][1] + coor[1][1] + coor[2][1] + coor[3][1]) / 4;
	
	if(st_UL)
		show_red_corner1(coor[0][0], coor[0][1]);
	if(st_UR)
		show_red_corner1(coor[1][0], coor[1][1]);
	if(st_DL)
		show_red_corner1(coor[2][0], coor[2][1]);
	if(st_DR)
		show_red_corner1(coor[3][0], coor[3][1]);
	// rt_kprintf("pic_i %d, pic_j %d\n", pic_i, pic_j);
	// rt_kprintf("%d %d %d %d\n", st_UL, st_UR, st_DL, st_DR);
	return 1;
}

int get_pic(void)
{
	memset(coor, 0, sizeof coor);
	uint32 begin, end;
	begin = rt_tick_get_millisecond();

	for (int i = 0; i < MT9V03X_CSI_W; ++i)
		for (int j = 0; j < MT9V03X_CSI_H; ++j)
			img_origin[i][j] = mt9v03x_csi_image[MT9V03X_CSI_H - 1 - j][i];
	// int thre = GetOSTU();
	for (int i = 0; i < MT9V03X_CSI_W; ++i)
		for (int j = 0; j < MT9V03X_CSI_H; ++j)
			// if(img_origin[i][j] < thre + 25)
			if (img_origin[i][j] < THRE)
				img_origin[i][j] = 0;
			else
				img_origin[i][j] = 255;
	ips114_displayimage032_zoomT(img_origin, MT9V03X_CSI_W, MT9V03X_CSI_H, 240, 135);

	sobel();
	// dialation();
	// memset(img, 0, sizeof img);
	// erosion();
	get_similarity();
	int flag = get_center();
	end = rt_tick_get_millisecond();
	// rt_kprintf("time %d\n", end - begin);

	for (int i = 0; i < MT9V03X_CSI_W; ++i)
		for (int j = 0; j < MT9V03X_CSI_H; ++j)
			img[i][j] *= 255;
	//ips114_displayimage032_zoomT(img, MT9V03X_CSI_W, MT9V03X_CSI_H, 240, 135);
	// ips114_displayimage032_zoom(mt9v03x_csi_image, MT9V03X_CSI_W, MT9V03X_CSI_H, 240, 135);
	//for (int i = 0; i < 4; i++)
	//	show_red_corner1(coor[i][0], coor[i][1]);
	return flag;
}

void show_red_corner1(int i, int j)
{
	uint16_t x_tmp = i * 240 / MT9V03X_CSI_W, y_tmp = (MT9V03X_CSI_H - j - 1) * 135 / MT9V03X_CSI_H;
	for (int i = x_tmp - 1; i < x_tmp + 2; ++i)
		for (int j = y_tmp - 1; j < y_tmp + 2; ++j)
			ips114_drawpoint(i, j, RED);
}