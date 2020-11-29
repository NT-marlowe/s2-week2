#include <stdio.h>

struct student
{
  //4+100+4+8+8 = 124, doubleに合わせる
  int id;
  char name[100];
  int age;
  double height;
  double weight;
};

struct tagged_student1
{
  //4+100+4+8+8+1 = 125, doubleに合わせる(8の倍数)ため0で3バイトパディング
  int id;
  char name[100];
  int age;
  double height;
  double weight;
  char tag;
};

struct tagged_student2
{
  char tag;
  int id;
  char name[100];
  int age;
  double height;
  double weight;
};

int main (int argc, char**argv)
{
  struct student s_begin;
  struct student s1;
  struct tagged_student1 s2;
  struct tagged_student2 s3;
  struct tagged_student2 s_end;

  /* 以下に各構造体の中身のポインタを表示するプログラムを書く */
  /* printf で %p フォーマットを指定する*/
  /* 逆順に表示（s_end, s3, s2, s1, s_begin の順）*/
  printf("s_end: %p\n", &s_end);
  printf("s3: %p\n", &s3);
  printf("s2: %p\n", &s2);
  printf("s1: %p\n", &s1);
  printf("s_begin: %p\n", &s_begin);
  // do_something!!

  /* 以下には各構造体のサイズをsizeof演算子で計算し、表示する */
  /* printf 表示には%ld を用いる*/
  printf("---------\n");
  printf("s_end:%ld\n", sizeof(s_end));
  printf("s3:%ld\n", sizeof(s3));
  printf("s2:%ld\n", sizeof(s2));
  printf("s1:%ld\n", sizeof(s1));
  printf("s_begin:%ld\n", sizeof(s_begin));
  // do_something!!
  
  return 0;
}
