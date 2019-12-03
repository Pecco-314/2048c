#include "2048.h"

int main()
{
    Init(MAXX, MAXY, BLOCK_SIZE);
    generate();
    generate();
    //set_block_value(0, 0, 1024);
    //MessageBox(NULL, "你赢了！", "2048", 0);
    while (1)
        ;
    return 0;
}

// 初始化，x行，y列，格子大小为bs
void Init(int x, int y, int bs)
{
    initUI(x, y, bs);
    init_map(x, y);
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
COORD get_coord(block *b)
{
    COORD coord;
    coord.X = 1 + (BLOCK_SIZE * 2 + 1) * b->y + (BLOCK_SIZE * 2 - cnt_digits(b->value) + 1) / 2;
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
void init_map(int x, int y)
{
    for (int i = 0; i < x; ++i)
        for (int j = 0; j < y; ++j)
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
    if (b->value == 0)
    {
        coord.X -= 3;
        locate(coord);
        for (int i = 0; i < BLOCK_SIZE * 2; ++i)
            putchar(' ');
    }
    else
    {
        locate(coord);
        printf("%d", b->value);
    }
}

// 初始化界面，棋盘为x行y列
void initUI(int x, int y, int bs)
{
    window_resize(x, y);
    system("title 2048"); //设置标题
    system("color F0");   //设置背景色为白色，前景色为黑色
    puts("2048控制台版 - made by Pecco\n");
    puts("W - 向上  A - 向左  S - 向下  D - 向右");
    print_board(x, y, bs);
}

// 对于x行y列的棋盘，设置窗口大小
void window_resize(int x, int y)
{
    char s[30] = "";
    int lines = 7 + x * (BLOCK_SIZE + 1);
    int cols = max(40, 3 + y * (BLOCK_SIZE * 2 + 1)); //cols不能小于40
    sprintf(s, "mode con cols=%d lines=%d", cols, lines);
    system(s);
}

//打印棋盘n行，列数为y，格子大小为bs，可定制各种格式
void print_board_line(const char head[], const char body[], const char crossing[], const char tail[], int y, int n, int bs)
{
    for (int k = 0; k < n; ++k)
    {
        fputs(head, stdout);
        for (int j = 0; j < y - 1; ++j)
        {
            for (int i = 0; i < bs * 2; ++i)
                fputs(body, stdout);
            fputs(crossing, stdout);
        }
        for (int i = 0; i < bs * 2; ++i)
            fputs(body, stdout);
        puts(tail);
    }
}

//打印棋盘头n行，列数为y，格子大小为bs
void print_board_head(int y, int n, int bs)
{
    print_board_line("┌", "─", "┬", "┐", y, n, bs);
}

//打印棋盘身n行，列数为y，格子大小为bs
void print_board_body(int y, int n, int bs)
{
    print_board_line("│", " ", "│", "│", y, n, bs);
}

//打印棋盘交叉处n行，列数为y，格子大小为bs
void print_board_crossing(int y, int n, int bs)
{
    print_board_line("├", "─", "┼", "┤", y, n, bs);
}

//打印棋盘尾n行，列数为y，格子大小为bs
void print_board_tail(int y, int n, int bs)
{
    print_board_line("└", "─", "┴", "┘", y, n, bs);
}

//打印x行y列的棋盘，格子大小为bs
void print_board(int x, int y, int bs)
{
    print_board_head(y, 1, bs);
    for (int i = 0; i < x - 1; ++i)
    {
        print_board_body(y, bs, bs);
        print_board_crossing(y, 1, bs);
    }
    print_board_body(y, bs, bs);
    print_board_tail(y, 1, bs);
}
