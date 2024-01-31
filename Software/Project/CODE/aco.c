#include "aco.h"

#define width 700
#define height 500
#define piece_step 15.0

const int inf = 0x3f3f3f3f;
#define N 30
#define iter_max 103
#define ant_max 52
#define MAX_N 1000
#define pieces 103

int ant_num;
int n, dis[N][N], px[N], py[N], viss[ant_max][N], clas[N];
int class[N];
int px_std[N], py_std[N];
int path[2][N << 1], path_tot[N << 1], path_predis[N * 3][pieces];
int tmpx, tmpy, num_path;

float32_t idis[N][N], pher[N][N], delta_pher[N][N];
int routes[ant_max][N], len_best[iter_max], path_best[iter_max][N];

bool chkmin(int *x, int y) { return (*x) > y ? (*x) = y, 1 : 0; }
// change the value of x and return true meanwhile

typedef struct {
	uint8_t x, y;
	int F, G, H, id;
} nodee;
nodee heap[MAX_N];
int sz, fa[MAX_N];

int calc_dis(int x1, int y1, int x2, int y2)
{
    int dx = x1 - x2, dy = y1 - y2;
    return sqrt(dx * dx + dy * dy);
}

#define ex_dis_L (30) 
#define ex_dis_R (15)
#define ex_dis_T (-20)
// ex_dis: means additional_distance in trans_task, measured in cm
int calc_tag(int x1, int y1, int x2, int y2, int tag) {
	int w = width, h = height;
	if (x1 == 0 && y1 == 0) {
		tmpx = 0, tmpy = 0;
		return calc_dis(0, 0, x2, y2);
	}

	if (tag == 0) { // to left side 
		if (x1 == 0 && x2 == 0) 
			tmpy = (y1 + y2) >> 1;
		tmpx = -ex_dis_L;
		tmpy = y1 + (int)(1.0 * (x1 + ex_dis_L) * (y2 - y1) / (x1 + x2 + 2 * ex_dis_L));
        tmpy = (tmpy + 30) / 60 * 60;
	}	

	if (tag == 1) { // to up side
		if (y1 == h && y2 == h)
			tmpx = (x1 + x2) >> 1;
		tmpy = h + ex_dis_T;
		tmpx = x1 + (int)(1.0 * (h + ex_dis_T - y1) * (x2 - x1) / (2 * (h + ex_dis_T) - y1 - y2));
	}
	
	if (tag == 2) { // to right side
		if (x1 == w && x2 == w) 
			tmpy = (y1 + y2) >> 1;
		tmpx = w + ex_dis_R;
		tmpy = y1 - (int)(1.0 * (w + ex_dis_R - x1) * (y1 - y2) / (2 * (w + ex_dis_R) - x1 - x2));
        tmpy = (tmpy + 30) / 60 * 60;
    
	}

	return calc_dis(x1, y1, tmpx, tmpy) + calc_dis(x2, y2, tmpx, tmpy);
}


// double alpha = 1.0, beta = 1, rho = 0, Q = 1;
// double alpha = 2, beta = 2, rho = 0.1, Q = 10;
double alpha = 1.1, beta = 5.0, rho = 0.1, Q = 1;
// double alpha = 2.1, beta = 8.8, rho = 0.1, Q = 100;
//double alpha = 2.1, beta = 8.8, rho = 0.1, Q = 100;

// alpha: level of pher engaged in decision (1 - 3) pher ^ alpha
// beta: level of idis engaged in decision (5 - 10) idis ^ beta
// rho: declining rate of pher remained (0 - 1)
// Q: level of pher remained when ant is passing (0 inf)

void init_tag(int tag)
{
    // tag = 0, normal
    // tag = 1, dis = (1/3) * distance towards 3 sides (run once)
    // tag = 2, dis = distance towards 1 exact side (run twice)
    
    if (tag == 0)
    {
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
            {
                if (i == j)
                    dis[i][j] = 0, idis[i][j] = 1.0 * (0x3f3f3f3f);
                else
                {
                    dis[i][j] = calc_dis(px[i], py[i], px[j], py[j]);
                    idis[i][j] = 100.0 / dis[i][j];
                }
                pher[i][j] = 1.0;
            }
    }

    if (tag == 1)
    {
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
            {
                if (i == j)
                    dis[i][j] = 0, idis[i][j] = 1.0 * (0x3f3f3f3f);
                else
                {
                    if (i == n - 1)
                        dis[i][j] = calc_dis(px[i], py[i], px[j], py[j]);
                    else
                    {
                        dis[i][j] = calc_tag(px[i], py[i], px[j], py[j], 0) + calc_tag(px[i], py[i], px[j], py[j], 1) + calc_tag(px[i], py[i], px[j], py[j], 2);
                        dis[i][j] /= 3;
                    }
                    idis[i][j] = 100.0 / dis[i][j];
                }
                pher[i][j] = 1.0;
            }
    }

    if (tag == 2)
    {
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
            {
                if (i == j)
                    dis[i][j] = 0, idis[i][j] = 1.0 * (0x3f3f3f3f);
                else {
                    dis[i][j] = calc_tag(px[i], py[i], px[j], py[j], clas[i]);
                    idis[i][j] = 100.0 / dis[i][j];
                }
                pher[i][j] = 1.0;
            }
    }

	for (int i = 0; i < n; ++ i) for (int j = 0; j < n; ++ j)
		idis[i][j] = pow(idis[i][j], beta);
}

double randn(double x, double y)
{ // generate a value between x and y
    double par = 1.0 * (rand() % 1000) / 1000;
    return par * (y - x) + x;
}

void shuffle(int *a, int n)
{ // reorganize array a randomly
    int tmp[13], vis[13];
    memset(tmp, 0, sizeof tmp);
    memset(vis, 0, sizeof vis);
    for (int t = 0, lst = 0; t < n; ++t)
    {
        lst += rand() % n, lst %= n;
        while (vis[lst])
            ++lst, lst %= n;
        tmp[t] = a[lst], vis[lst] = 1;
    }
    for (int i = 0; i < n; ++i)
        a[i] = tmp[i];
}

int len[ant_max], vis[N], unvis[N], been[N];
double P[N];
int ant(int tot_iter, int m, int st_point, int back_st, int tim, int tag) {
	// tag: 0, 1, 2, same definity
	// tim: tim-th route

	// st_point: -1 means randn, else means a concrete starting point
	// back_st: 1 means back to starting point finally, 0 means no back
	memset(delta_pher, 0, sizeof delta_pher);
	memset(len_best, 0, sizeof len_best);
	for (int iter = 0; iter < tot_iter; ++ iter) { // iter-th iteration
		if (st_point != -1) 
			for (int i = 0; i < m; ++ i) routes[i][0] = st_point;
			// all from st_point
		else {
			// all throwed in graph randomly
			int tmp[ant_max], rds = m % n == 0 ? m / n : m / n + 1, tot_ant = 0;
			for (int i = 0; i < rds; ++ i) 
				for (int j = tim; j < n; ++ j) tmp[tot_ant ++] = j;
			shuffle(tmp, tot_ant);
			for (int i = 0; i < m; ++ i) routes[i][0] = tmp[i]; 
		}

		for (int j = 1; j < n; ++ j) for (int i = 0; i < m; ++ i) { // j-th move, i-th ant
			int _unvis = 0;
			double Psum = 0, rate = 0, cur_rate = 0;
			memset(vis, 0, sizeof vis);
			memset(unvis, 0, sizeof unvis);
			memset(been, 0, sizeof been);
			memset(P, 0, sizeof P);

			for (int k = 0; k < j; ++ k) been[routes[i][k]] = 1;
			for (int k = 0; k < n; ++ k) if (!been[k]) unvis[_unvis ++] = k;

			for (int k = 0; k < _unvis; ++ k) 
				Psum += P[k] = pher[routes[i][j - 1]][unvis[k]] * idis[routes[i][j - 1]][unvis[k]];
			rate = randn(0.0, Psum);
			for (int k = 0; k < _unvis; ++ k) {
				cur_rate += P[k];
				if (cur_rate > rate) {
					routes[i][j] = unvis[k];
					break;
				}
			}
		}
		memset(len, 0, sizeof len);
		for (int i = 0; i < m; ++ i) {
			for (int j = 0; j < n - 1; ++ j)
				len[i] += dis[routes[i][j]][routes[i][j + 1]];
			//len[i] += back_st * dis[routes[i][n - 1]][routes[i][0]];
		}
		int min_len = len[0], sum_len = len[0], min_index = 0;
		for (int i = 1; i < m; ++ i) { // for i-th ant
			sum_len += len[i];
			if (chkmin(&min_len, len[i])) min_index = i;
		}
		len_best[iter] = min_len;
		for (int i = 0; i < n; ++ i)
			path_best[iter][i] = routes[min_index][i];
		for (int i = 0; i < m; ++ i) {
			for (int j = 0; j < n - 1; ++ j) {
				delta_pher[routes[i][j]][routes[i][j + 1]] += 1.0 * Q / len[i];
				delta_pher[routes[i][j + 1]][routes[i][j]] = delta_pher[routes[i][j]][routes[i][j + 1]];
			}
			/*
			if (back_st) { 
				delta_pher[routes[i][n - 1]][routes[i][0]] += 1.0 * Q / len[i];
				delta_pher[routes[i][0]][routes[i][n - 1]] = delta_pher[routes[i][n - 1]][routes[i][0]];
			}
			*/
		}
		for (int i = 0; i < n; ++ i) 
			for (int j = 0; j < n; ++ j)
				pher[i][j] = (1 - rho) * pher[i][j] + delta_pher[i][j];
		memset(routes, 0, sizeof routes);
	}

	int ans_len = 0x3f3f3f3f, ans_iter_index = 0;
	
	if (tag == 0 && back_st == 1) { // no_trans_task
		for (int i = 0; i < tot_iter; ++ i) {
			int lst = path_best[i][n - 1];
			int tmp = len_best[i] + calc_dis(px[lst], py[lst], width + ex_dis_R, height + ex_dis_T);
			if (chkmin(&ans_len, tmp)) ans_iter_index = i;

		}
		for(int i = 0; i < n; ++ i)
			path[tim][i] = path_best[ans_iter_index][i];
	}

	if (tag == 1 && back_st == 1) { // run_once
		for (int i = 0; i < tot_iter; ++ i) {
			int lst = path_best[i][n - 1], tmp = 0;
			tmp += calc_tag(px[lst], py[lst], 0, 0, 0);
			tmp += calc_tag(px[lst], py[lst], 0, 0, 1);
			tmp += calc_tag(px[lst], py[lst], 0, 0, 2);
			tmp = len_best[i] + tmp / 3;
			if (chkmin(&ans_len, tmp)) ans_iter_index = i;
		}
		for(int i = 0; i < n; ++ i)
			path[tim][i] = path_best[ans_iter_index][i];
	}

	if (tag == 2 && back_st == 0) { // run_twice1
		for (int i = 1; i < tot_iter; ++ i)
			if (chkmin(&ans_len, len_best[i])) ans_iter_index = i;
		for(int i = 0; i < n; ++ i)
			path[tim][i] = path_best[ans_iter_index][i];
	}

	if (tag == 2 && back_st == 1) { // run_twice2
		for (int i = 0; i < tot_iter; ++ i) {
			int lst = path_best[i][n - 1];
			int tmp = len_best[i] + calc_tag(px[lst], py[lst], 0, 0, clas[lst]);
			if (chkmin(&ans_len, tmp)) ans_iter_index = i;
		}
		for(int i = 0; i < n; ++ i)
			path[tim][i] = path_best[ans_iter_index][i];
	}
	return ans_len; 
}

int ant_ori(int tot_iter, int m, int st_point, int back_st, int tim, int tag) {
	// tag: 0, 1, 2, same definity
	// tim: tim-th route

	// st_point: -1 means randn, else means a concrete starting point
	// back_st: 1 means back to starting point finally, 0 means no back
	memset(delta_pher, 0, sizeof delta_pher);
	memset(len_best, 0, sizeof len_best);
	for (int iter = 0; iter < tot_iter; ++ iter) { // iter-th iteration
		if (st_point != -1) 
			for (int i = 0; i < m; ++ i) routes[i][0] = st_point;
			// all from st_point
		else {
			// all throwed in graph randomly
			int tmp[ant_max], rds = m % n == 0 ? m / n : m / n + 1, tot_ant = 0;
			for (int i = 0; i < rds; ++ i) 
				for (int j = tim; j < n; ++ j) tmp[tot_ant ++] = j;
			shuffle(tmp, tot_ant);
			for (int i = 0; i < m; ++ i) routes[i][0] = tmp[i]; 
		}
		memset(viss, 0, sizeof 0);
		for (int j = 1; j < n; ++ j) for (int i = 0; i < m; ++ i) { // j-th move, i-th ant
			int vis[N], unvis[N], been[N], _vis = j, _unvis = 0;
			double P[N], Psum = 0, rate = 0, cur_rate = 0;
			memset(vis, 0, sizeof vis);
			memset(unvis, 0, sizeof unvis);
			memset(been, 0, sizeof been);
			memset(P, 0, sizeof P);
			for (int k = 0; k < j; ++ k) been[vis[k] = routes[i][k]] = 1;
			for (int k = 0; k < n; ++ k) 
				if (!been[k]) unvis[_unvis ++] = k;
			for (int k = 0; k < _unvis; ++ k) 
				Psum += P[k] = 1.0 * (1e11) * pow(pher[vis[_vis - 1]][unvis[k]], alpha) * pow(idis[vis[_vis - 1]][unvis[k]], beta);
			rate = randn(0.0, Psum);
			for (int k = 0; k < _unvis; ++ k) {
				cur_rate += P[k];
				if (cur_rate > rate) {
					routes[i][j] = unvis[k];
					break;
				}
			}
		}
		int len[ant_max];
		memset(len, 0, sizeof len);
		for (int i = 0; i < m; ++ i) {
			for (int j = 0; j < n - 1; ++ j)
				len[i] += dis[routes[i][j]][routes[i][j + 1]];
			//len[i] += back_st * dis[routes[i][n - 1]][routes[i][0]];
		}
		int min_len = len[0], sum_len = len[0], min_index = 0;
		for (int i = 1; i < m; ++ i) { // for i-th ant
			sum_len += len[i];
			if (chkmin(&min_len, len[i])) min_index = i;
		}
		len_best[iter] = min_len;
		for (int i = 0; i < n; ++ i)
			path_best[iter][i] = routes[min_index][i];
		for (int i = 0; i < m; ++ i) {
			for (int j = 0; j < n - 1; ++ j) {
				delta_pher[routes[i][j]][routes[i][j + 1]] += 1.0 * Q / len[i];
				delta_pher[routes[i][j + 1]][routes[i][j]] = delta_pher[routes[i][j]][routes[i][j + 1]];
			}
			if (back_st) { 
				delta_pher[routes[i][n - 1]][routes[i][0]] += 1.0 * Q / len[i];
				delta_pher[routes[i][0]][routes[i][n - 1]] = delta_pher[routes[i][n - 1]][routes[i][0]];
			}
		}
		for (int i = 0; i < n; ++ i) 
			for (int j = 0; j < n; ++ j)
				pher[i][j] = (1 - rho) * pher[i][j] + delta_pher[i][j];
		memset(routes, 0, sizeof routes);
	}

	int ans_len = 0x3f3f3f3f, ans_iter_index = 0;
	
	if (tag == 0 && back_st == 1) { // no_trans_task
		for (int i = 0; i < tot_iter; ++ i) {
			int lst = path_best[i][n - 1];
			int tmp = len_best[i] + calc_dis(px[lst], py[lst], 0, 0);
			if (chkmin(&ans_len, tmp)) ans_iter_index = i;
		}
		for(int i = 0; i < n; ++ i)
			path[tim][i] = path_best[ans_iter_index][i];
	}

	if (tag == 1 && back_st == 1) { // run_once
		for (int i = 0; i < tot_iter; ++ i) {
			int lst = path_best[i][n - 1], tmp = 0;
			tmp += calc_tag(px[lst], py[lst], 0, 0, 0);
			tmp += calc_tag(px[lst], py[lst], 0, 0, 1);
			tmp += calc_tag(px[lst], py[lst], 0, 0, 2);
			tmp = len_best[i] + tmp / 3;
			if (chkmin(&ans_len, tmp)) ans_iter_index = i;
		}
		for(int i = 0; i < n; ++ i)
			path[tim][i] = path_best[ans_iter_index][i];
	}

	if (tag == 2 && back_st == 0) { // run_twice1
		for (int i = 1; i < tot_iter; ++ i)
			if (chkmin(&ans_len, len_best[i])) ans_iter_index = i;
		for(int i = 0; i < n; ++ i)
			path[tim][i] = path_best[ans_iter_index][i];
	}

	if (tag == 2 && back_st == 1) { // run_twice2
		for (int i = 0; i < tot_iter; ++ i) {
			int lst = path_best[i][n - 1];
			int tmp = len_best[i] + calc_tag(px[lst], py[lst], 0, 0, clas[lst]);
			if (chkmin(&ans_len, tmp)) ans_iter_index = i;
		}
		for(int i = 0; i < n; ++ i)
			path[tim][i] = path_best[ans_iter_index][i];
	}
	return ans_len; 
}





// return 1: need yaw_dis (back more
// return 2: need hori_dis

int check_yaw(int x0, int y0, int x1, int y1) {
	double dx = x1 - x0, dy = y1 - y0, yaw;
	yaw = dx == 0 ? (dy > 0 ? 0 : 180) : 90 - atan(fabs(dy / dx)) * 180 / 3.14159;	
	if (dx > 0 && dy < 0) yaw = 180 - yaw;
	if (dx < 0 && dy < 0) yaw = 180 + yaw;
	if (dx < 0 && dy >= 0) yaw = 360 - yaw;
	double ds = sqrt(dx * dx + dy * dy);

    if (60 < ds) {
        if (89 < yaw && yaw < 136) 
            return 1;
        
        if (224 < yaw && yaw < 271) 
            return 1;

		if (136 < yaw && yaw < 224)
			return 2;
    }

    return 0;
}

void make_path(int x0, int y0, int x1, int y1, int num, double dis_step) {
	// tim means tim-th run in method 3
	double dx = x1 - x0, dy = y1 - y0, yaw;
	yaw = dx == 0 ? (dy > 0 ? 0 : 180) : 90 - atan(fabs(dy / dx)) * 180 / 3.14159;	
	if (dx > 0 && dy < 0) yaw = 180 - yaw;
	if (dx < 0 && dy < 0) yaw = 180 + yaw;
	if (dx < 0 && dy >= 0) yaw = 360 - yaw;

	double ds = sqrt(dx * dx + dy * dy);
	int tot = ds / dis_step + 1;
	double step_x = dx / tot, step_y = dy / tot;

	if (path_tot[num] == 0) 
		path_x[num][0] = x0, path_y[num][0] = y0, path_predis[num][0] = 0;
    
	for (int i = path_tot[num] + 1; i <= path_tot[num] + tot; ++ i) {
		path_x[num][i] = path_x[num][i - 1] + step_x;
		path_y[num][i] = path_y[num][i - 1] + step_y;
		path_predis[num][i] = path_predis[num][i - 1] + calc_dis(0, 0, (int)step_x, (int)step_y);
	//	path_yaw[num][i - 1] = yaw;
	}

	path_tot[num] += tot;
}

void push(nodee ps) { // key1: F, key2: H;
	int now = sz++; // id of ps to push
	while (now > 0) {
		int f = (now - 1) >> 1; // id of father
		nodee fa = heap[f];
		if (fa.F < ps.F) break;
		if (fa.F == ps.F && fa.H < ps.H) break;
		// if no toggle then break;
		heap[now] = heap[f], now = f; 
	}
	heap[now] = ps;
}

nodee pop() {
	nodee res = heap[0], x = heap[-- sz];
	int rt = 0;
	while (rt * 2 + 1 < sz) {
		int lchild = 2 * rt + 1, rchild = 2 * rt + 2;
		if (rchild < sz && heap[rchild].F < heap[lchild].F) lchild = rchild;
		if (rchild < sz && heap[rchild].F == heap[lchild].F && heap[rchild].H < heap[lchild].H) lchild = rchild;
		if (heap[lchild].F >= x.F) break;
		if (heap[lchild].F == x.F && heap[lchild].H > x.H) break;
		heap[rt] = heap[lchild], rt = lchild; 
	}
	heap[rt] = x;
	return res;
}

#define NN 80
#define MM 60
#define ww 76
#define hh 56 // (x / 10 + 3 * 2)
int stx, sty, edx, edy, mm;
int obs[NN][MM], fax[NN][MM], fay[NN][MM], cst[NN][MM];
int dir0[8][2] = {-1, -1, 1, 1, 1, -1, -1, 1, 1, 0, 0, 1, -1, 0, 0, -1}; 
int dir1[8][2] = {1, 2, -1, 2, 1, -2, -1, -2, 2, 1, 2, -1, -2, 1, -2, -1}; 
int dir2[8][2] = {3, 2, -3, 2, 3, -2, -3, -2, 2, 3, 2, -3, -2, 3, -2, -3};
int dir3[8][2] = {1, 3, -1, 3, 1, -3, -1, -3, 3, 1, 3, -1, -3, 1, -3, -1}; 

void astar_init() {
    memset(obs, 0, sizeof obs);
    for (int i = 0; i < pic_num; ++ i) {
        int x = pos_int[i].x, y = pos_int[i].y;
        x = x * 2 - 1 + 3;
        y = y * 2 - 1 + 3;
        for (int xx = x - 1; xx < x + 3; ++ xx) 
            for (int yy = y - 1; yy < y + 3; ++ yy)
                obs[xx][yy] = 1;
    }
    obs[4][4] = obs[4][5] = obs[5][5] = obs[5][4] = 0;
}

int roux[pieces], rouy[pieces], num_;

void astar(int stx, int sty, int edx, int edy, int ipath) {
    sz = 0, num_ = 0;
    memset(roux, 0, sizeof roux);
    memset(rouy, 0, sizeof rouy);
    memset(cst, 0, sizeof cst);
    memset(fax, 0, sizeof fax);
    memset(fay, 0, sizeof fay);
    
	nodee st;
	st.x = stx, st.y = sty;
	st.G = 0, st.H = calc_dis(stx, sty, edx, edy);
	st.F = st.G + st.H;
	push(st);

    int if_find = 0;
	while (sz) {
		nodee cur = pop();
        nodee nxt;
		//cerr << sz << endl;
		if (cur.x == edx && cur.y == edy) {

			int x = edx, y = edy;
			while (true) {
                roux[num_] = x, rouy[num_] = y, ++ num_;
				if (x == stx && y == sty) break;
				int tmpx = fax[x][y], tmpy = fay[x][y];
				x = tmpx, y = tmpy;
			}
            if_find = 1;
			break;
		}

        int flag = 0;
		for (int i = 0; i < 4; ++ i) flag *= (1 - obs[cur.x + dir1[i][0]][cur.y + dir1[i][1]]); 
		for (int i = 0; i < 8; ++ i) {  // for dir0, 8 sides
			nxt.x = cur.x + dir0[i][0], nxt.y = cur.y + dir0[i][1];
			if (obs[nxt.x][nxt.y] == 1) continue;
			if (nxt.x < 1 || nxt.x > ww || nxt.y < 1 || nxt.y > hh) continue;

			nxt.G = cur.G + (abs(dir0[i][0]) + abs(dir0[i][1]) == 1 ? 10 : 14);
			nxt.H = calc_dis(nxt.x, nxt.y, edx, edy);
			nxt.F = nxt.G + nxt.H;

			if (cst[nxt.x][nxt.y] == 0 || cst[nxt.x][nxt.y] > nxt.F) {
				cst[nxt.x][nxt.y] = nxt.F, push(nxt);
				fax[nxt.x][nxt.y] = cur.x;
				fay[nxt.x][nxt.y] = cur.y;
			}
		}

        flag = 1;
		for (int i = 0; i < 8; ++ i) flag *= (1 - obs[cur.x + dir1[i][0]][cur.y + dir1[i][1]]); 
		if (flag) for (int i = 0; i < 8; ++ i) { // for dir1, move as (|1|, |2|)
			nxt.x = cur.x + dir1[i][0], nxt.y = cur.y + dir1[i][1];
			if (obs[nxt.x][nxt.y] == 1) continue;
			if (nxt.x < 1 || nxt.x > ww || nxt.y < 1 || nxt.y > hh) continue;

			nxt.G = cur.G + 22;
			nxt.H = calc_dis(nxt.x, nxt.y, edx, edy);
			nxt.F = nxt.G + nxt.H;

			if (cst[nxt.x][nxt.y] == 0 || cst[nxt.x][nxt.y] > nxt.F) {
				cst[nxt.x][nxt.y] = nxt.F, push(nxt);
				fax[nxt.x][nxt.y] = cur.x;
				fay[nxt.x][nxt.y] = cur.y;
			}
		}

		flag = 1;
		for (int i = 0; i < 8; ++ i) flag *= (1 - obs[cur.x + dir2[i][0]][cur.y + dir2[i][1]]); 
		if (flag) for (int i = 0; i < 8; ++ i) { // for dir2, move as (|2|, |3|)
			nxt.x = cur.x + dir2[i][0], nxt.y = cur.y + dir2[i][1];
			if (obs[nxt.x][nxt.y] == 1) continue;
			if (nxt.x < 1 || nxt.x > ww || nxt.y < 1 || nxt.y > hh) continue;

			nxt.G = cur.G + 36;
			nxt.H = calc_dis(nxt.x, nxt.y, edx, edy);
			nxt.F = nxt.G + nxt.H;

			if (cst[nxt.x][nxt.y] == 0 || cst[nxt.x][nxt.y] > nxt.F) {
				cst[nxt.x][nxt.y] = nxt.F, push(nxt);
				fax[nxt.x][nxt.y] = cur.x;
				fay[nxt.x][nxt.y] = cur.y;
			}
		}
		
        flag = 1;
		for (int i = 0; i < 8; ++ i) flag *= (1 - obs[cur.x + dir3[i][0]][cur.y + dir3[i][1]]); 
		if (flag) for (int i = 0; i < 8; ++ i) { // for dir2, move as (|1|, |3|)
			nodee nxt; 
			nxt.x = cur.x + dir3[i][0], nxt.y = cur.y + dir3[i][1];
			if (obs[nxt.x][nxt.y] == 1) continue;
			if (nxt.x < 1 || nxt.x > ww || nxt.y < 1 || nxt.y > hh) continue;

			nxt.G = cur.G + 32;
			nxt.H = calc_dis(nxt.x, nxt.y, edx, edy);
			nxt.F = nxt.G + nxt.H;

			if (cst[nxt.x][nxt.y] == 0 || cst[nxt.x][nxt.y] > nxt.F) {
				cst[nxt.x][nxt.y] = nxt.F, push(nxt);
				fax[nxt.x][nxt.y] = cur.x;
				fay[nxt.x][nxt.y] = cur.y;
			}
		}
	}

    if (if_find == 0)
        //rt_kprintf("!!!!!!!!!!!!!!!!!FUCK (%d, %d) to (%d, %d)\n", stx, sty, edx, edy);
				;
    for (int i = num_ - 1; i > 0; -- i) {
        make_path(roux[i] * 10 - 45, rouy[i] * 10 - 45, roux[i - 1] * 10 - 45, rouy[i - 1] * 10 - 45, num_path, piece_step);
    }
    make_path(roux[0] * 10 - 45, rouy[0] * 10 - 45, px[path[0][ipath]], py[path[0][ipath]], num_path, piece_step);
	++ num_path;
	return;
}

void no_trans_task()
{   //manual:
    // back to (0, 0)
    // ant(tot_iter, m, st_point, back_st, tim, tag)
    
    /* codes below for debug without detect A4 map */
        //px[0] = 3, py[0] = 3;
        //px[1] = 2, py[1] = 9;
        //px[2] = 5, py[2] = 17;
        //px[3] = 7, py[3] = 13;
        //px[4] = 10, py[4] = 10;

        n = pic_num;
        // consider the distance between camera lens and the car body center
        for(int i = 0; i < n; i++)
            px[i] = (px[i] - 1) * 20 + 10 - erx, py[i] = (py[i] - 1) * 20 + 10 - ery;
    /* end here */

    px[n] = py[n] = 0, n = n + 1;
    init_tag(0);
    int ans_len = ant(20, 50, n - 1, 1, 0, 0);

//    int tot_dis = 0; // includes back to (0, 0)
//    for (int i = 0; i < n - 1; ++i)
//    tot_dis += calc_dis(px[path[0][i]], py[path[0][i]], px[path[0][i + 1]], py[path[0][i + 1]]);
//    tot_dis += calc_dis(px[path[0][n - 1]], py[path[0][n - 1]], 0, 0);

    // px[i], py[i]
    astar_init();
    for (int i = 0; i < n - 1; ++i) {
        
        int x1 = (px[path[0][i]] + 39) / 10 + 1, y1 = (py[path[0][i]] + 39) / 10 + 1, x2 = (px[path[0][i + 1]] + 39) / 10 + 1, y2 = (py[path[0][i + 1]] + 39) / 10 + 1;
        //rt_kprintf("(%d, %d) to (%d, %d)\n", x1, y1, x2, y2);
        astar(x1, y1, x2, y2, i + 1);
        //make_path(px[path[0][i]], py[path[0][i]], px[path[0][i + 1]], py[path[0][i + 1]], num_path++, piece_step);
    }
    //astar((px[path[0][n - 1]] + 39) / 10 + 1, (py[path[0][n - 1]] + 39) / 10 + 1, 4, 4, 0);
    make_path(px[path[0][n - 1]], py[path[0][n - 1]], 0, 0, num_path++, piece_step);
//    uart_putfloat(USART_1, path_x[9][path_tot[9]]);rt_kprintf(",");
//    uart_putfloat(USART_1, path_y[9][path_tot[9]]);rt_kprintf("\n");
}

void run_once_nouse()
{
    // ant(tot_iter, m, st_point, back_st, tim, tag)
    px[n] = py[n] = 0, n = n + 1;
    init_tag(1);
    int ans_len = ant(20, 50, n - 1, 1, 0, 1);

    int tot_dis = calc_dis(0, 0, px[path[0][1]], py[path[0][1]]);
    make_path(0, 0, px[path[0][1]], py[path[0][1]], num_path++, piece_step);

    for (int i = 1; i < n - 1; ++i)
    {
        int x1 = px[path[0][i]], y1 = py[path[0][i]];
        int x2 = px[path[0][i + 1]], y2 = py[path[0][i + 1]];

        tot_dis += calc_tag(x1, y1, x2, y2, clas[path[0][i]]);
        make_path(x1, y1, tmpx, tmpy, num_path++, piece_step);
        make_path(tmpx, tmpy, x2, y2, num_path++, piece_step);
    }
    tot_dis += calc_tag(px[path[0][n - 1]], py[path[0][n - 1]], 0, 0, clas[path[0][n - 1]]);
    make_path(px[path[0][n - 1]], py[path[0][n - 1]], tmpx, tmpy, num_path++, piece_step);
    make_path(tmpx, tmpy, 0, 0, num_path++, piece_step);
    if (clas[path[0][n - 1]] == 0)
        --num_path;
}

int lstpos = 0;
void run_twice1()
{
    n = pic_num;
    // consider the distance between camera lens and the car body center
    for(int i = 0; i < n; i++)
        px[i] = (px[i] - 1) * 20 + 10 - erx, py[i] = (py[i] - 1) * 20 + 10 - ery;
    // ant(tot_iter, m, st_point, back_st, tim, tag)
    px[n] = py[n] = 0, n = n + 1;
    init_tag(0);
    int tot1 = 0, tot2 = 0;
    tot1 = ant(20, 50, n - 1, 0, 0, 2);

    for (int i = 0; i < n - 1; ++i)
        make_path(px[path[0][i]], py[path[0][i]], px[path[0][i + 1]], py[path[0][i + 1]], num_path, piece_step), num_path ++;

    lstpos = path[0][n - 1];
}

void run_twice2() {
    --n, init_tag(2);
    int tot2 = ant(20, 50, lstpos, 1, 1, 2);
    for (int i = 0; i < n - 1; ++i)
    {
        int x1 = px[path[1][i]], y1 = py[path[1][i]];
        int x2 = px[path[1][i + 1]], y2 = py[path[1][i + 1]];
        tot2 += calc_tag(x1, y1, x2, y2, clas[path[1][i]]);
        make_path(x1, y1, tmpx, tmpy, num_path++, piece_step);
        make_path(tmpx, tmpy, x2, y2, num_path++, piece_step);
    }
    tot2 += calc_tag(px[path[1][n - 1]], py[path[1][n - 1]], 0, 0, clas[path[1][n - 1]]);
    make_path(px[path[1][n - 1]], py[path[1][n - 1]], tmpx, tmpy, num_path++, piece_step);
    make_path(tmpx, tmpy, 0, 0, num_path++, piece_step);

    if (clas[path[1][n - 1]] == 0)
        --num_path;
}

/*********************************************************
	BELOW FOR ACO with remained structure
  ******************************************************/

int lsttt = 0, nxttt = 0, vis_[N], lstttt = 0;
int path_clk = 1;

void ACO_to_first(void)
{
    n = pic_num;
    // consider the distance between camera lens and the car body center
    for(int i = 0; i < n; ++ i) {
        px[i] = (px[i] - 1) * 20 + 10 - erx, py[i] = (py[i] - 1) * 20 + 10 - ery;
        px_std[i] = px[i], py_std[i] = py[i];
    }
	
	px[n] = 0, py[n] = 0, n = n + 1;
	init_tag(0);
		
	ant(100, 50, n - 1, 1, 0, 0);	
	n = n - 1;
	shuffle(path[0], 13);
	lsttt = path[0][path_clk];
	rt_kprintf("last %d\n", path[0][13]);
    lstttt = 0;
    vis_[lsttt] = 1;
    py[lsttt] += 10;
    make_path(0, 0, px[lsttt], py[lsttt], num_path++, piece_step);
    //rt_kprintf("(%d, %d) to (%d, %d), num:%d\n", 0, 0, px[lsttt], py[lsttt], num_path - 1);
}

#define yaw_dis (2)
void ACO_to_next(void)
{
	nxttt = path[0][++ path_clk];
	int lst_check = check_yaw(px[lsttt], py[lsttt], px[nxttt], py[nxttt]);

	if (lst_check == 0) {
        make_path(px[lsttt], py[lsttt], px[nxttt], py[nxttt], num_path++, piece_step);
	}

	if (lst_check == 1) {
        py[nxttt] -= yaw_dis;
        make_path(px[lsttt], py[lsttt], px[nxttt], py[nxttt], num_path++, piece_step);
	}

	if (lst_check == 2) {
        int hori_dis = px[nxttt] <= px[lsttt] ? 20 : -20; 
        make_path(px[lsttt], py[lsttt], px[nxttt] + hori_dis, py[nxttt], num_path++, piece_step);
		int tmp = px[nxttt] <= px[lsttt] ? -8 : 8;
        make_path(px[nxttt] + hori_dis, py[nxttt], px[nxttt] + tmp, py[nxttt], num_path++, piece_step);
		px[nxttt] += tmp;
	}

    lstttt = lsttt;
	lsttt = nxttt;
}

void ACO_to_R() { // tag = 2, drop fruit(car bottom side)
	px_std[n] = px[n] = 725, py_std[n] = py[n] = 460;

	int lst_check = check_yaw(px[n - 1], py[n - 1], px[n], py[n]);
	if (lst_check != 1) 
        make_path(px[n - 1], py[n - 1], px[n], py[n], num_path++, piece_step);
	if (lst_check == 1) 
        py[n] -= yaw_dis,
        make_path(px[n - 1], py[n - 1], px[n], py[n], num_path++, piece_step); 
	lstttt = lsttt;
	lsttt = n;
	++ n;
}

void ACO_to_T() { // tag = 1, drop traffic(car right side)
	px_std[n] = px[n] = 465, py_std[n] = py[n] = 500;
	int lst_check = check_yaw(px[n - 1], py[n - 1], px[n], py[n]);
	if (lst_check != 1) 
        make_path(px[n - 1], py[n - 1], px[n], py[n], num_path++, piece_step);
	if (lst_check == 1) 
        py[n] -= yaw_dis,
        make_path(px[n - 1], py[n - 1], px[n], py[n], num_path++, piece_step);
	lsttt = n;
	++ n;
}

void ACO_to_L() { // tag = 0, drop animals(car left side)
	px_std[n] = px[n] = -10, py_std[n] = py[n] = 400;
	int lst_check = check_yaw(px[n - 1], py[n - 1], px[n], py[n]);
	if (lst_check != 1) 
        make_path(px[n - 1], py[n - 1], px[n], py[n], num_path++, piece_step);
	if (lst_check == 1) 
        py[n] -= yaw_dis,
        make_path(px[n - 1], py[n - 1], px[n], py[n], num_path++, piece_step);
	lsttt = n;
	++ n;
}

void to_first(void)
{
    n = pic_num;
    // consider the distance between camera lens and the car body center
    for(int i = 0; i < n; ++ i) {
        px[i] = (px[i] - 1) * 20 + 10 - erx, py[i] = (py[i] - 1) * 20 + 10 - ery;
        px_std[i] = px[i], py_std[i] = py[i];
				clas[i] = (class[i] - 1) / 5 + 1;
        if(clas[i] == 1) clas[i] = 0;
        if(clas[i] == 3) clas[i] = 1;
    }
		

    int MAXXX = 10000;
	for (int i = 0; i < n; ++ i)
		if (chkmin(&MAXXX, calc_dis(0, 0, px[i], py[i]))) lsttt = i;
	
    lstttt = 0;
    vis_[lsttt] = 1;
    py[lsttt] += 10;
    make_path(0, 0, px[lsttt], py[lsttt], num_path++, piece_step);
    //rt_kprintf("(%d, %d) to (%d, %d), num:%d\n", 0, 0, px[lsttt], py[lsttt], num_path - 1);
}

int find_(int nx, int ny, int np) {
	// np fangle qu xiayige 
	int MAXX = 10000, id = 0;
	for (int i = 0; i < n; ++ i) if (!vis_[i]) {
		int val = calc_tag(nx, ny, px[i], py[i], clas[np]);
		if (chkmin(&MAXX, val)) id = i;
	}
	vis_[id] = 1;
	return id;
}

void to_next(void)
{
	nxttt = find_(px[lsttt], py[lsttt], lsttt);

	calc_tag(px[lsttt], py[lsttt], px[nxttt], py[nxttt], clas[lsttt]);

    int lst_check = check_yaw(px[lsttt], py[lsttt], tmpx, tmpy);
    tmpy -= lst_check * yaw_dis;
    make_path(px[lsttt], py[lsttt], tmpx, tmpy, num_path++, piece_step);
    //rt_kprintf("(%d, %d) to (%d, %d), num:%d\n", px[lsttt], py[lsttt], tmpx, tmpy, num_path - 1);
    tmpy += lst_check * yaw_dis;

    if (clas[lsttt] != 1) { 
        //py[nxttt] -= check_yaw(tmpx, tmpy, px[nxttt], py[nxttt]) * yaw_dis;
        py[nxttt] -= lst_check * yaw_dis;
        make_path(tmpx, tmpy, px[nxttt], py[nxttt], num_path++, piece_step);

    } else {


        // change here
        int hori_dis = px[nxttt] <= tmpx ? 20 : -20; 

        py[nxttt] -= check_yaw(tmpx, tmpy, px[nxttt] + hori_dis, py[nxttt]) * yaw_dis;
        make_path(tmpx, tmpy, px[nxttt] + hori_dis, py[nxttt], num_path++, piece_step);
			int tmp = px[nxttt] <= tmpx ? -8 : 8;
        make_path(px[nxttt] + hori_dis, py[nxttt], px[nxttt] + tmp, py[nxttt], num_path++, piece_step);
			px[nxttt] += tmp;
        //++ num_path;
    }
    //rt_kprintf("(%d, %d) to (%d, %d), num:%d\n", tmpx, tmpy, px[nxttt], py[nxttt], num_path - 1);
    //rt_kprintf("\n\n");
    tmpy -= lst_check * yaw_dis;
    lstttt = lsttt;
	lsttt = nxttt;
}
	
void go_home(void)
{
    px[n] = 5, py[n] = 0;
	calc_tag(px[nxttt], py[nxttt], 0, 0, clas[nxttt]);
    tmpy -= check_yaw(px[nxttt], py[nxttt], tmpx, tmpy) * yaw_dis;
    make_path(px[nxttt], py[nxttt], tmpx, tmpy, num_path++, piece_step);

    //rt_kprintf("(%d, %d) to (%d, %d), num:%d\n", px[nxttt], py[nxttt], tmpx, tmpy, num_path - 1);
    make_path(tmpx, tmpy, 5, 0, num_path++, piece_step);
    lstttt = lsttt;
    lsttt = n;
}
