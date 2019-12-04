#ifndef _2048_H_
#define _2048_H_
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define swap(a, b) \
    if (a != b)    \
    a ^= b ^= a ^= b
#define MAXX 4
#define MAXY 4
#define BLOCK_SIZE 3
#define GOAL 2048
#define ROF 10 //出现4的频率的倒数（Reciprocal of Frequency）

// 方块，包含横坐标，纵坐标，值和可结合性
typedef struct
{
    int x, y, value, combinable;
} block;

//游戏地图，存储方块指针
block *map[MAXX][MAXY];

void Init();
void Step();

int cnt_digits(int n);

void prepare_to_input();
void empty_input_area();
void move(int ch);

void new_game();
void judge();
int judge_lose();
int judge_win();
void lose();
void win();

int has_space();
void reprint_all();
void init_map();
void generate();
void all_combinable();
void forall_d(int procedure(block *, char, int *), int *location);
void forall_u(int procedure(block *, char, int *), int *location);
void forall_l(int procedure(block *, char, int *), int *location);
void forall_r(int procedure(block *, char, int *), int *location);
int all_move(char dir);

void set_block_value(int i, int j, int value);
void print_block(int i, int j);
block *new_block(int x, int y);
block *get_neighbor(const block *b, char dir);
block *get_end(block *bk, char dir);
int combine_to(block *b, char dir);
int move_to(block *b, char dir, int *location);
COORD get_coord(const block *b);

void initUI();
void window_resize();
void window_fix();
void locate(COORD coord);
void hide_cursor();

void print_board_line(const char head[], const char body[], const char crossing[], const char tail[], int y, int n);
void print_board_head(int y, int n);
void print_board_body(int y, int n);
void print_board_crossing(int y, int n);
void print_board_tail(int y, int n);
void print_board();
#endif