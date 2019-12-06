#ifndef _2048_H_
#define _2048_H_
#define max(a, b) ({int _a = a; int _b = b; _a > _b ? _a : _b; })
#define swap(a, b) ({int *_a = &a; int *_b = &b; int _t = *_a; *_a = *_b; *_b = _t; }
#define ROF 10 //����4��Ƶ�ʵĵ�����Reciprocal of Frequency��
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <stdarg.h>

// ���飬���������꣬�����꣬�ɽ�����Լ��ı�
typedef struct
{
    int x, y, combinable;
    int value; ///
    //char *text;
} block;

//��Ϸ��ͼ���洢����ָ�룬��̬�����ڴ�
block ***Map;

//�����¼���洢ע��ķ���
typedef struct
{
    unsigned char id, is_genetatable;
    char text[100];
    double freq;
} block_record;
block_record *Regs[100];

//������void����ָ��
typedef void (*fp)();

//Ƶ�ʱ�
int Freq[100];

//��ϱ�
int Comb[100][100];

//��Ϸ����߷ֺ���ͷ�
unsigned long long Pts, Bests;

//����̨������������
int Lines, Cols;

//��.config�ж�ȡ�Ĳ���
int MAXX, MAXY, GOAL, BLOCK_SIZE;
char AUTO_SAVE;

void Init();
void Step();

void do_nothing();
void quit_game();
void open_config_then_load_game();

int count_digits(int n);

void init_config();
void open_config();
void init_block_config(FILE *config);
void register_block(int id, char *text, char is_genetatable, double freq);
void init_combination_config();

void prepare_to_input();
void empty_input_area();
void process_input(int ch);
void input_for_choice(const char *input, ...);

void save_game();
void load_game();
void new_game();
void judge();
int judge_lose();
int judge_win();
void lose();
void win();
void play_twinkle_star();

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
void set_text_color(int color);
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

void warn_dangerous_new_game();
void warn_unmatch_save_format(int x, int y);

#endif