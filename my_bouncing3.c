#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "physics2.h"
#define bufsize 100

void my_plot_objects(Object objs[], const size_t numobj, const double t, const Condition cond);
void my_update_velocities_and_positions(Object objs[], const size_t numobj, const Condition cond);
void my_update_positions(Object objs[], const size_t numobj, const Condition cond);
void my_bounce(Object objs[], const size_t numobj, const Condition cond);
void my_merge(Object objs[], const size_t numobj, const Condition cond);
int is_inside(double x, double y, const Condition cond);

int main(int argc, char **argv)
{
  const Condition cond = {
		    .width  = 75,
		    .height = 40,
		    .G = 1.0,
		    .dt = 0.10,
		    .cor = 0.8
  };
  // 惑星の融合を観察するために、dtを小さくしてある。a.txtとplanets.datの両方で動作を確認した。
  size_t objnum;
  FILE *fp;
  if (argc < 3) {
    printf("Input numobj and filename\n");
    return EXIT_FAILURE;
  } 
  else if (argc > 3) {
    printf("Too many arguments\n");
    return EXIT_FAILURE;
  } 
  else {
    objnum = atoi(argv[1]);
    fp = fopen(argv[2], "r");
    if (fp == NULL) {
      printf("File %s not found\n", argv[2]);
      return EXIT_FAILURE;
    }
  }
  Object objects[objnum];
  char buf[bufsize];
  // int cnt_obj = 0;
  int idx = 0;
  while (fgets(buf, bufsize, fp) != NULL && idx <= objnum) {
    //行が多すぎたら読み込みを打ち切る
    if (buf[0] == '#') continue;
    if (sscanf(buf, "%lf %lf %lf %lf %lf", &objects[idx].m, &objects[idx].x, &objects[idx].y, 
    &objects[idx].vx, &objects[idx].vy) == 5) {
      idx++;
    }   
  }
  if (idx < objnum) {
    //行が足りなかったら0で埋める
    for (int i = idx; i < objnum; i++) {
      objects[i].m = 0;
      objects[i].x = objects[i].y = 0;
      objects[i].vx = objects[i].vy = 0;
    }
  }

  // シミュレーション. ループは整数で回しつつ、実数時間も更新する
  const double stop_time = 400;
  double t = 0;
  printf("\n");
  for (int i = 0 ; t <= stop_time ; i++){
    t = i * cond.dt;
    my_merge(objects, objnum, cond);
    my_update_velocities_and_positions(objects, objnum, cond);
    // my_update_positions(objects, objnum, cond);
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
    if (X >= 0 && X < w && Y >= 0 && Y < h && objs[k].m > 0) {
      cell[Y][X] = 1;
    };
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

void my_update_velocities_and_positions(Object objs[], const size_t numobj, const Condition cond) {
  // slackでの指摘の通り修正した
  double accel_x[numobj], accel_y[numobj];
  for (int i = 0; i < numobj; i++) {
    double ax = 0, ay = 0;
    double d = 0;
    for (int j = 0; j < numobj; j++) {
      if (i == j) continue;
      d = sqrt(pow(objs[i].x-objs[j].x, 2) + pow(objs[i].y-objs[j].y, 2));
      /*if (d < 1) {
        my_merge(objs[i], objs[j], numobj, cond);
        continue;
      }*/
      ax += objs[j].m  * (objs[j].x - objs[i].x) / (d*d*d);
      ay += objs[j].m  * (objs[j].y - objs[i].y) / (d*d*d);
    }
    accel_x[i] = cond.G * ax;
    accel_y[i] = cond.G * ay;
  }

  for (int i = 0; i < numobj; i++) {
    objs[i].prev_x = objs[i].x;
    objs[i].prev_y = objs[i].y;
    objs[i].x += objs[i].vx * cond.dt;
    objs[i].y += objs[i].vy * cond.dt;
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
    if (objs[i].m < 1) continue;
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

void my_merge(Object objs[], const size_t numobj, const Condition cond) {
  for (int i = 0; i < numobj; i++) {
      Object newobj;
      double d;
      for (int j = 0; j < numobj; j++) {
        if (i == j) continue;
        d = sqrt(pow(objs[i].x-objs[j].x, 2) + pow(objs[i].y-objs[j].y, 2));
        if (d >= 1) continue;
        newobj.m = objs[i].m + objs[j].m;
        newobj.x = (objs[i].x + objs[j].x) / 2;
        newobj.y = (objs[i].y + objs[j].y) / 2;
        newobj.prev_x = (objs[i].prev_x + objs[j].prev_x) / 2;
        newobj.prev_y = (objs[i].prev_y + objs[j].prev_y) / 2;
        newobj.vx = (objs[i].m*objs[i].vx + objs[i].m*objs[j].vx) / newobj.m;
        newobj.vy = (objs[i].m*objs[i].vy + objs[i].m*objs[j].vy) / newobj.m;

        objs[i] = newobj;

        objs[j].m = 0;
        objs[j].x = objs[j].prev_x = -1000; 
        objs[j].y = objs[j].prev_y = -1000;
        objs[j].vx = objs[j].vy = 0;
      }
    }
}