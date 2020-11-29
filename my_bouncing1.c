#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "physics2.h"

void my_plot_objects(Object objs[], const size_t numobj, const double t, const Condition cond);
void my_update_velocities(Object objs[], const size_t numobj, const Condition cond);
void my_update_positions(Object objs[], const size_t numobj, const Condition cond);
void my_bounce(Object objs[], const size_t numobj, const Condition cond);
int is_inside(double x, double y, const Condition cond);

int main(int argc, char **argv)
{
  const Condition cond = {
		    .width  = 75,
		    .height = 40,
		    .G = 1.0,
		    .dt = 1.0,
		    .cor = 0.8
  };
  
  size_t objnum = 3;
  Object objects[objnum];

  // objects[1] は巨大な物体を画面外に... 地球のようなものを想定
  objects[0] = (Object){ .m = 60.0, .x = 0, .y = -19.9, .prev_x = 0, .prev_y = -19.9, .vx = 0.0, .vy = 2.0};
  //objects[1] = (Object){ .m = 100000.0,.x = 0, .y =  1000.0, .prev_x = 0, .prev_y = 1000.0, .vx = 0.0, .vy = 0.0};
  objects[1] = (Object){ .m = 100, .x = 3, .y = -10, .prev_x = 3, .prev_y = -10, .vx = 2.1, .vy = 2.0};
  objects[2] = (Object){ .m = 200, .x = -2, .y = -15, .prev_x = -2, .prev_y = -15, .vx = 3.5, .vy = 2.0};

  // シミュレーション. ループは整数で回しつつ、実数時間も更新する
  const double stop_time = 400;
  double t = 0;
  printf("\n");
  for (int i = 0 ; t <= stop_time ; i++){
    t = i * cond.dt;
    my_update_velocities(objects, objnum, cond);
    my_update_positions(objects, objnum, cond);
    my_bounce(objects, objnum, cond);
    
    // 表示の座標系は width/2, height/2 のピクセル位置が原点となるようにする
    my_plot_objects(objects, objnum, t, cond);
    
    usleep(200 * 1000); // 200 x 1000us = 200 ms ずつ停止
    printf("\e[%dA", cond.height+2);// 壁とパラメータ表示分で3行
  }
  return EXIT_SUCCESS;
}

// 実習: 以下に my_ で始まる関数を実装する
// 最終的に phisics2.h 内の事前に用意された関数プロトタイプをコメントアウト
void my_plot_objects(Object objs[], const size_t numobj, const double t, const Condition cond) {
  int w = cond.width, h = cond.height;
  int cell[h][w];
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) cell[i][j] = 0;
  }
  int Y = 0, X = 0;
  for (int k = 0; k < numobj; k++) {
    X = objs[k].x + w/2;
    Y = objs[k].y + h/2;
    if (X >= 0 && X < w && Y >= 0 && Y < h) {cell[Y][X] = 1;};
  }
  
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      if (cell[i][j] == 1) printf("o");
      else printf(" ");
    }
    printf("\n");
  }

  printf("-----\n");
  printf("t =   %.2f, ", t);
  for (int i = 0; i < numobj; i++) {
    printf("objs[%d].vx = %.2f, objs[%d].vy = %.2f, ", i, objs[i].vx, i, objs[i].vy);
  }
  printf("\n");
}

void my_update_velocities(Object objs[], const size_t numobj, const Condition cond) {
  double accel_x[numobj], accel_y[numobj];
  for (int i = 0; i < numobj; i++) {
    double ax = 0, ay = 0;
    double d = 0;
    for (int j = 0; j < numobj; j++) {
      if (i == j) continue;
      d = sqrt(pow(objs[i].x-objs[j].x, 2) + pow(objs[i].y-objs[j].y, 2));
      if (d == 0) printf("d = 0\n");
      ax += objs[j].m  * (objs[j].x - objs[i].x) / (d*d*d);
      ay += objs[j].m  * (objs[j].y - objs[i].y) / (d*d*d);
    }
    accel_x[i] = cond.G * ax;
    accel_y[i] = cond.G * ay;
  }
  for (int i = 0; i < numobj; i++) {
    objs[i].vx += accel_x[i] * cond.dt;
    objs[i].vy += accel_y[i] * cond.dt;
  }
}

void my_update_positions(Object objs[], const size_t numobj, const Condition cond) {
  for (int i = 0; i < numobj; i++) {
    objs[i].prev_x = objs[i].x;
    objs[i].prev_y = objs[i].y;
    objs[i].x += objs[i].vx * cond.dt;
    objs[i].y += objs[i].vy * cond.dt;
  }
}

int is_inside(double x, double y, const Condition cond) {
  int X = x + (cond.width/2);
  int Y = y + (cond.height/2);
  return X >= 0 && X < cond.width && Y >= 0 && Y < cond.height;
}

void my_bounce(Object objs[], const size_t numobj, const Condition cond) {
  double X, Y;
  int w = cond.width, h = cond.height;
  for (int i = 0; i < numobj; i++) {
    if (!is_inside(objs[i].prev_x, objs[i].prev_y, cond)) continue;
    X = objs[i].x + (w/2);
    Y = objs[i].y + (h/2);
    if (X < 0) {
      double after_t = cond.dt - (objs[i].prev_x + w/2) / fabs(objs[i].vx);
      objs[i].vx *= -cond.cor;
      objs[i].x = fabs(objs[i].vx) * after_t - w/2;
    } 
    else if (X >= w) {
      double after_t = cond.dt - (w/2 - objs[i].prev_x) / fabs(objs[i].vx);
      objs[i].vx *= -cond.cor;
      objs[i].x = w/2 - fabs(objs[i].vx) * after_t;
    } 
    if (Y < 0) {
      double after_t = cond.dt - (objs[i].prev_y + h/2) / fabs(objs[i].vy);
      objs[i].vy *= -cond.cor;
      objs[i].y = fabs(objs[i].vy) * after_t - h/2;
    } 
    else if (Y >= h) {
      double after_t = cond.dt - (h/2 - objs[i].prev_y) / fabs(objs[i].vy);
      objs[i].vy *= -cond.cor;
      objs[i].y = h/2 - fabs(objs[i].vy) * after_t;
    }
  }
}
