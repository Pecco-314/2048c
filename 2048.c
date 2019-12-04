#include "2048.h"

int main()
{
    Init();
    while (1)
        Step();
    return 0;
}

// 初始化
void Init()
{
    initUI();
    init_map();
    new_game();
}

// 处理每一步的操作
void Step()
{
    prepare_to_input();
    char ch = getch();
    if (ch == -32)
        ch = getch();
    //printf("%d", ch);///
    //TODO:移动、合并
    judge();
    if (has_space())
        generate();
}

//把光标移到输入区
void prepare_to_input()
{
    COORD coord = {0, (BLOCK_SIZE + 1) * MAXX + 4};
    locate(coord);
}

//判断游戏胜负情况
void judge()
{
    if (has_space())
        return;
    else if (judge_win())
        win();
    else if (judge_lose())
        lose();
}

//判断是否胜利，胜利返回1
int judge_win()
{
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            if (map[i][j]->value == GOAL)
                return 1;
    return 0;
}

//判断是否失败，失败返回1
int judge_lose()
{
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            if ((i < MAXX - 1 && map[i + 1][j]->value == map[i][j]->value) ||
                (j < MAXY - 1 && map[i][j + 1]->value == map[i][j]->value))
                return 0;
    return 1;
}

//游戏失败处理
void lose()
{
    puts("已无可移动的方块，游戏失败。\n N - 新游戏  C - 继续游戏");
    char c = getch();
    while (1)
    {
        switch (c)
        {
        case 'N':
        case 'n':
            new_game();
        case 'C':
        case 'c':
            empty_input_area();
            return;
        }
        c = getch();
    }
}

//游戏胜利处理
void win()
{
    puts("游戏胜利，恭喜！\n N - 新游戏  C - 继续游戏");
    char c = getch();
    while (1)
    {
        switch (c)
        {
        case 'N':
        case 'n':
            new_game();
        case 'C':
        case 'c':
            empty_input_area();
            return;
        }
        c = getch();
    }
}

//清空输入区
void empty_input_area()
{
    prepare_to_input();
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < max(40, 3 + MAXY * (BLOCK_SIZE * 2 + 1)); ++j)
            putchar(' ');
    prepare_to_input();
}

// 新游戏，清空棋盘，随机生成两个方块
void new_game()
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            set_block_value(i, j, 0);
    generate();
    generate();
}

// 返回一个十进制整数的位数
int cnt_digits(int n)
{
    int digits = 0;
    while (n)
    {
        digits++;
        n /= 10;
    }
    return digits;
}

// 设置光标位置
void locate(COORD coord)
{
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// 传入方块指针，返回实际的光标位置
COORD get_coord(const block *b)
{
    COORD coord;
    if (b->value)
        coord.X = 1 + (BLOCK_SIZE * 2 + 1) * b->y + (BLOCK_SIZE * 2 - cnt_digits(b->value) + 1) / 2;
    else
        coord.X = 1 + (BLOCK_SIZE * 2 + 1) * b->y;
    coord.Y = 3 + (BLOCK_SIZE + 1) * b->x + (BLOCK_SIZE + 1) / 2;
    return coord;
}

//创建新方块
block *new_block(int x, int y)
{
    block *b = (block *)malloc(sizeof(block));
    b->x = x;
    b->y = y;
    b->value = 0;
    return b;
}

//查询有无空位
int has_space()
{
    int suc = 0;
    for (int i = 0; i < MAXX; ++i)
    {
        for (int j = 0; j < MAXY; ++j)
        {
            if (map[i][j]->value == 0)
            {
                suc = 1;
                break;
            }
        }
        if (suc)
            break;
    }
    return suc;
}

//随机生成2或4
void generate()
{
    srand(time(NULL) * clock());
    int v = rand() % ROF ? 2 : 4;
    while (1)
    {
        int x = rand() % MAXX, y = rand() % MAXY;
        if (map[x][y]->value == 0)
        {
            set_block_value(x, y, v);
            break;
        }
    }
}

//初始化游戏地图数组
void init_map()
{
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            map[i][j] = new_block(i, j);
}

// 设置某坐标的方块的值，并打印
int set_block_value(int i, int j, int value)
{
    map[i][j]->value = value;
    print_block(i, j);
}

// 打印某坐标的方块，如果方块的值是0则清空该格
void print_block(int i, int j)
{
    block *b = map[i][j];
    COORD coord = get_coord(b);
    locate(coord);
    if (b->value == 0)
        for (int i = 0; i < BLOCK_SIZE * 2; ++i)
            putchar(' ');
    else
        printf("%d", b->value);
}

// 初始化界面，棋盘为x行y列
void initUI()
{
    window_resize();
    window_fix();
    system("title 2048"); //设置标题
    system("color F0");   //设置背景色为白色，前景色为黑色
    puts("2048控制台版 - made by Pecco\n");
    puts("W - 向上  A - 向左  S - 向下  D - 向右");
    print_board();
}

// 对于x行y列的棋盘，设置窗口大小
void window_resize()
{
    char s[30] = "";
    int lines = 8 + MAXX * (BLOCK_SIZE + 1);
    int cols = max(40, 3 + MAXY * (BLOCK_SIZE * 2 + 1)); //cols不能小于40
    sprintf(s, "mode con cols=%d lines=%d", cols, lines);
    system(s);
}

//使窗口大小不可改变
void window_fix()
{
    SetWindowLongPtrA(
        GetConsoleWindow(),
        GWL_STYLE,
        GetWindowLongPtrA(GetConsoleWindow(), GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX);
}

//打印棋盘n行，列数为y，可定制各种格式
void print_board_line(const char head[], const char body[], const char crossing[],
                      const char tail[], int y, int n)
{
    for (int k = 0; k < n; ++k)
    {
        fputs(head, stdout);
        for (int j = 0; j < y - 1; ++j)
        {
            for (int i = 0; i < BLOCK_SIZE * 2; ++i)
                fputs(body, stdout);
            fputs(crossing, stdout);
        }
        for (int i = 0; i < BLOCK_SIZE * 2; ++i)
            fputs(body, stdout);
        puts(tail);
    }
}

//打印棋盘头n行，列数为y
void print_board_head(int y, int n)
{
    print_board_line("┌", "─", "┬", "┐", y, n);
}

//打印棋盘身n行，列数为y
void print_board_body(int y, int n)
{
    print_board_line("│", " ", "│", "│", y, n);
}

//打印棋盘交叉处n行，列数为y
void print_board_crossing(int y, int n)
{
    print_board_line("├", "─", "┼", "┤", y, n);
}

//打印棋盘尾n行，列数为y
void print_board_tail(int y, int n)
{
    print_board_line("└", "─", "┴", "┘", y, n);
}

//打印棋盘
void print_board()
{
    print_board_head(MAXY, 1);
    for (int i = 0; i < MAXX - 1; ++i)
    {
        print_board_body(MAXY, BLOCK_SIZE);
        print_board_crossing(MAXY, 1);
    }
    print_board_body(MAXY, BLOCK_SIZE);
    print_board_tail(MAXY, 1);
}
