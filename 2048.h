#ifndef _2048_H_
#define _2048_H_
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define MAXX 4
#define MAXY 4
#define BLOCK_SIZE 5
#define ROF = 10 //出现4的频率的倒数（Reciprocal of Frequency）

// 方块，包含横坐标，纵坐标和值
typedef struct
{
    int x, y, value;
} block;

//游戏地图，存储16个方块指针
block *map[MAXX][MAXY];

void Init(int x, int y, int bs);
int cnt_digits(int n);
int set_block_value(int i, int j, int value);
int has_space();
void generate();
void print_block(int i, int j);
block *new_block(int x, int y, int value);
void init_map(int x, int y);
void initUI(int x, int y, int bs);
void locate(int x, int y);
void locate(COORD coord);
void window_resize(int x, int y);
void print_board_line(const char head[], const char body[], const char crossing[], const char tail[], int y, int n, int bs);
void print_board_head(int y, int n, int bs);
void print_board_body(int y, int n, int bs);
void print_board_crossing(int y, int n, int bs);
void print_board_tail(int y, int n, int bs);
void print_board(int x, int y, int bs);
COORD get_coord(block *b);
#endif