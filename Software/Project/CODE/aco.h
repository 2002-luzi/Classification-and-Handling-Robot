/**********************************************************************
TSP question solved by Ant Colony Optimization
Module function:
    knowing n points and their coordinates
    find a almost optimal permutaion to minimize the tot_distance of routes
Key: ant colony optimization; routes fitting

Meaning of variables:

    path[N]: record the final best routes
    path_tot[t]: t-th path into tot pieces
    path_yaw[t]: t-th path's direction
    path_x/y[t][N]: record the position during routes; t means t-th path, N means the number of data in t-th path

    dis[N][N]: matrix of distance
    idis[N][N]: reciprocal matrix of distance
    ans_len: final length
    ans_iter_index: record which iteration perform the best

    pher[i][j]: the level of pheromones from i-th point to j-th
    delta_pher[i][j]: incremental of pher that ants remained on i -> j in a itertion
    routes[u][i]: record u-th ant's route in a iteration

    len_best[i]: the best performance an ant created in i-th iteration
    path_best[i][N]: record the best ant's route in i-th iteration

NOTICE
    n <= 20: the number of points (so n - 1 is the number of pitctures on the floor, the first point is the position where the car start)
    m <= 200: the number of ants
    tot_iter <= 100: iteration times

    for further simulating, constants below should be modified
    but make sure that n * n * m * iterations < 1e9 on pc, 1e7 on mcu

    no memory limit
**********************************************************************/
#ifndef _ACO_H
#define _ACO_H

#include <headfile.h>

#define ery 45
#define erx 10
#define NN 30

extern int ant_num;
extern int px[NN],py[NN], clas[NN], class[NN];
extern int path_tot[NN << 1];
extern int path[2][NN << 1];
extern int vis_[NN], lsttt, nxttt, lstttt;
extern int tmpx, tmpy;
extern int px_std[NN], py_std[NN];
extern int tot_len;

void no_trans_task();
void run_once();
void run_twice1();
void run_twice2();
void make_path(int x0, int y0, int x1, int y1, int num, double dis_step);
void to_first(void);
void to_next(void);
void go_home(void);
void ACO_to_R();
void ACO_to_T();
void ACO_to_L();


#endif
