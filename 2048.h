#ifndef _2048_H_
#define _2048_H_
#define max(a, b) ({int _a = a; int _b = b; _a > _b ? _a : _b; })
#define swap(a, b) ({int *_a = &a; int *_b = &b; int _t = *_a; *_a = *_b; *_b = _t; })
#define MAXX 4
#define MAXY 4
#define GOAL 2048
#define BLOCK_SIZE 3
#define AUTO_SAVE 1
#define ROF 10 //出现4的频率的倒数（Reciprocal of Frequency）
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

///警告枚举
enum warning
{
    UNMATCH_SAVE_FORMAT
};

// 方块，包含横坐标，纵坐标，值和可结合性
typedef struct
{
    int x, y, value, combinable;
} block;

//游戏地图，存储方块指针
block *Map[MAXX][MAXY];

//游戏的最高分和最低分
unsigned long long Pts, Bests;

//控制台的行数和列数
int Lines, Cols;

void Init();
void Step();

int count_digits(int n);

void prepare_to_input();
void empty_input_area();
void process_input(int ch);

void save_game();
void load_game();
void new_game();
void judge();
int judge_lose();
int judge_win();
void lose();
void win();

void init_map();
void generate();
void all_combinable();
int forall_d(int procedure(block *, char));
int forall_u(int procedure(block *, char));
int forall_l(int procedure(block *, char));
int forall_r(int procedure(block *, char));
int all_move(char dir);
int has_space();

void set_block_value(int i, int j, int value);
void print_block(int i, int j);
block *new_block(int x, int y);
block *get_neighbor(const block *b, char dir);
block *get_end(block *bk, char dir);
int combine_to(block *b, char dir);
int move_to(block *b, char dir);
COORD get_coord(const block *b);

void initUI();
void window_resize();
void window_fix();
void locate(COORD coord);
void hide_cursor();
void set_text_color(int ForeColor, int BackColor);
void color_puts(char *string, int color);

void init_hist();
void save_history();
int count_lines(char *filename);
void delete_last_line(char *filename);
void update_from_last_line();
void withdraw_step();

void print_board_line(const char head[], const char body[], const char crossing[], const char tail[], int y, int n);
void print_board_head(int y, int n);
void print_board_body(int y, int n);
void print_board_crossing(int y, int n);
void print_board_tail(int y, int n);
void print_board();
void reprint_all();
void update_pts();

void trigger_warning(enum warning w);
void warn_unmatch_save_format();

#endif