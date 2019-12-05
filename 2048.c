#include "2048.h"

// 初始化
void Init()
{
    initUI();
    init_map();
    load_game();
}

// 处理每一步的操作
void Step()
{
    prepare_to_input();
    int ch = getch();
    if (ch == 224 || ch == 0)
        ch = getch() + 1000;
    move(ch);
    reprint_all();
    if (AUTO_SAVE)
        save_game();
    judge();
}

//保存游戏
void save_game()
{
    system("attrib -r save.dat"); //使存档文件可写
    FILE *save = fopen("save.dat", "w");
    fprintf(save, "%d %d ", MAXX, MAXY); //存储当前棋盘的大小
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            fprintf(save, "%d ", map[i][j]->value);
    fprintf(save, "%llu %llu", pts, bests);
    fclose(save);
    system("attrib +r save.dat"); //设置存档文件只读
}

//读取游戏
void load_game()
{
    FILE *save = fopen("save.dat", "r");
    if (save != NULL)
    {
        int x, y;
        fscanf(save, "%d%d", &x, &y);
        for (int i = 0; i < MAXX; ++i)
            for (int j = 0; j < MAXY; ++j)
                fscanf(save, "%d", &map[i][j]->value);
        fscanf(save, "%llu%llu", &pts, &bests);
        fclose(save);
        if (x != MAXX || y != MAXY)
            trigger_warning(UNMATCH_SAVE_FORMAT);
        reprint_all();
        save_game();
    }
    else
        new_game();
}

//根据输入处理移动
void move(int ch)
{
    char c = 0;
    if (ch == 'S' || ch == 's' || ch == 1080)
        c = 'd';
    else if (ch == 'W' || ch == 'w' || ch == 1072)
        c = 'u';
    else if (ch == 'A' || ch == 'a' || ch == 1075)
        c = 'l';
    else if (ch == 'D' || ch == 'd' || ch == 1077)
        c = 'r';
    else if (ch == 1063)
        new_game();
    else if (ch == 1059)
        WinExec("notepad.exe help.txt", SW_SHOW);
    if (c && all_move(c))
        generate();
}

//获取方块在某个方向上紧挨着的方块，若达边界返回NULL
block *get_neighbor(const block *b, char dir)
{
    switch (dir)
    {
    case 'l':
        return b->y == 0 ? NULL : map[b->x][b->y - 1];
    case 'r':
        return b->y == MAXY - 1 ? NULL : map[b->x][b->y + 1];
    case 'u':
        return b->x == 0 ? NULL : map[b->x - 1][b->y];
    case 'd':
        return b->x == MAXX - 1 ? NULL : map[b->x + 1][b->y];
    }
}

//得到方块往某个方向移动应该到达的位置
block *get_end(block *bk, char dir)
{
    block *b = bk;
    while (get_neighbor(b, dir) != NULL && get_neighbor(b, dir)->value == 0)
        b = get_neighbor(b, dir);
    return b;
}

//方块试图与某个方向上的相邻方块结合，结合成功返回1，否则返回0
int combine_to(block *b, char dir)
{
    block *end = get_neighbor(b, dir);
    if (end != NULL && b->value == end->value && end->combinable)
    {
        end->value *= 2;
        b->value = 0;
        end->combinable = 0;
        pts += end->value;
        bests = max(bests, pts);
        return 1;
    }
    return 0;
}

//方块往某个方向移动，并试图结合，移动或结合成功返回1，否则返回0
int move_to(block *b, char dir)
{
    int sec = 0;
    block *end = get_end(b, dir);
    if (end != b && b->value)
    {
        end->value = b->value;
        b->value = 0;
        sec = 1;
    }
    if (combine_to(end, dir))
        sec = 1;
    return sec;
}

//从下往上遍历所有方块
int forall_d(int procedure(block *, char))
{
    int sec = 0;
    for (int i = MAXX - 1; i >= 0; --i)
        for (int j = 0; j < MAXY; ++j)
            sec = max(sec, procedure(map[i][j], 'd'));
    return sec;
}

//从上往下遍历所有方块
int forall_u(int procedure(block *, char))
{
    int sec = 0;
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            sec = max(sec, procedure(map[i][j], 'u'));
    return sec;
}

//从左往右遍历所有方块
int forall_l(int procedure(block *, char))
{
    int sec = 0;
    for (int j = 0; j < MAXY; ++j)
        for (int i = MAXX - 1; i >= 0; --i)
            sec = max(sec, procedure(map[i][j], 'l'));
    return sec;
}

//从右往左遍历所有方块
int forall_r(int procedure(block *, char))
{
    int sec = 0;
    for (int j = MAXY - 1; j >= 0; --j)
        for (int i = MAXX - 1; i >= 0; --i)
            sec = max(sec, procedure(map[i][j], 'r'));
    return sec;
}

// 全部往某个方向移动，若移动或组合成功则返回1
int all_move(char dir)
{
    all_combinable();
    int sec = 0;
    switch (dir)
    {
    case 'd':
        sec = max(sec, forall_d(move_to));
        break;
    case 'u':
        sec = max(sec, forall_u(move_to));
        break;
    case 'l':
        sec = max(sec, forall_l(move_to));
        break;
    case 'r':
        sec = max(sec, forall_r(move_to));
    }
    return sec;
}

//把光标移到输入区
void prepare_to_input()
{
    COORD coord = {0, (BLOCK_SIZE + 1) * MAXX + 6};
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
            if ((i < MAXX - 1 && map[i + 1][j]->value == map[i][j]->value) || (j < MAXY - 1 && map[i][j + 1]->value == map[i][j]->value))
                return 0;
    return 1;
}

//游戏失败处理
void lose()
{
    prepare_to_input();
    puts("已无可移动的方块，游戏失败。\n N - 新游戏  C - 继续游戏");
    char c = getch();
    while (1)
    {
        switch (c)
        {
        case 'N':
        case 'n':
            new_game(); //FALLTROUGH
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
    prepare_to_input();
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

//使所有方块可结合
void all_combinable()
{
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            map[i][j]->combinable = 1;
}

//清空输入区
void empty_input_area()
{
    prepare_to_input();
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < cols; ++j)
            putchar(' ');
    prepare_to_input();
}

// 新游戏，清空棋盘，随机生成两个方块，分数设置为0
void new_game()
{
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            set_block_value(i, j, 0);
    generate();
    generate();
    save_game();
    pts = 0;
    reprint_all();
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
    coord.X = 1 + (BLOCK_SIZE * 2 + 1) * b->y;
    coord.Y = 4 + (BLOCK_SIZE + 1) * b->x + BLOCK_SIZE / 2;
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
    int sec = 0;
    for (int i = 0; i < MAXX; ++i)
    {
        for (int j = 0; j < MAXY; ++j)
        {
            if (map[i][j]->value == 0)
            {
                sec = 1;
                break;
            }
        }
        if (sec)
            break;
    }
    return sec;
}

// 重新打印所有方块，并更新分数和最高分
void reprint_all()
{
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            print_block(i, j);
    update_pts();
}

// 更新分数和最高分
void update_pts()
{
    COORD coord = {7, (BLOCK_SIZE + 1) * MAXX + 4};
    locate(coord); //定位到打印分数的位置
    printf("%-*llu", cols - 7, pts);
    coord.Y += 1;
    locate(coord);
    printf("%-*llu", cols - 7, bests);
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

// 设置某坐标的方块的值
void set_block_value(int i, int j, int value)
{
    map[i][j]->value = value;
}

// 打印某坐标的方块，如果方块的值是0则清空该格
void print_block(int i, int j)
{
    block *b = map[i][j];
    COORD coord = get_coord(b);
    locate(coord);
    for (int i = 0; i < BLOCK_SIZE * 2; ++i)
        putchar(' ');
    if (map[i][j]->value)
    {
        coord.X += (BLOCK_SIZE * 2 - cnt_digits(b->value) + 1) / 2; //居中对齐
        locate(coord);
        printf("%d", b->value);
    }
}

// 初始化界面，棋盘为x行y列
void initUI()
{
    window_resize();
    window_fix();
    hide_cursor();
    system("title 2048"); //设置标题
    system("color F0");   //设置背景色为白色，前景色为黑色
    puts("2048控制台版 - made by Pecco\n");
    puts("若您在游玩时有任何问题，可以按F1打开帮助文件");
    print_board();
}

// 根据格子的多少和大小设置窗口大小
void window_resize()
{
    char s[30] = "";
    lines = 9 + MAXX * (BLOCK_SIZE + 1);
    cols = max(46, 3 + MAXY * (BLOCK_SIZE * 2 + 1)); // cols不能小于46
    sprintf(s, "mode con cols=%d lines=%d", cols, lines);
    system(s);
}

//使窗口大小不可改变
void window_fix()
{
    SetWindowLongPtrA(
        GetConsoleWindow(), GWL_STYLE,
        GetWindowLongPtrA(GetConsoleWindow(), GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX);
}

//隐藏光标
void hide_cursor()
{
    CONSOLE_CURSOR_INFO cursor;
    cursor.bVisible = FALSE;
    cursor.dwSize = sizeof(cursor);
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorInfo(handle, &cursor);
}

//打印棋盘n行，列数为y，可定制各种格式
void print_board_line(const char head[], const char body[], const char crossing[],
                      const char tail[], int y, int n)
{
    for (int k = 0; k < n; ++k)
    {
        printf("%s", head);
        for (int j = 0; j < y - 1; ++j)
        {
            for (int i = 0; i < BLOCK_SIZE * 2; ++i)
                printf("%s", body);
            printf("%s", crossing);
        }
        for (int i = 0; i < BLOCK_SIZE * 2; ++i)
            printf("%s", body);
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
    puts("分数：");
    puts("最高：");
}

void trigger_warning(enum warning w)
{
    switch (w)
    {
    case UNMATCH_SAVE_FORMAT:
        warn_unmatch_save_format();
    }
}

void warn_unmatch_save_format()
{
    prepare_to_input();
    color_puts("错误：存档的棋盘大小与配置不统一\n N - 新游戏", 4);
    char c = getch();
    while (1)
    {
        switch (c)
        {
        case 'N':
        case 'n':
            new_game();
            empty_input_area();
            return;
        }
        c = getch();
    }
}

//设置文字颜色
// 0-黑 1-蓝 2-绿 3-浅绿 4-红 5-紫 6-黄 7-白 8-灰 9-淡蓝
// 10-淡绿 11-淡浅绿 12-淡红 13-淡紫 14-淡黄 15-亮白
void set_text_color(int ForeColor, int BackColor)
{
    HANDLE winHandle; //句柄
    winHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(winHandle, ForeColor + BackColor * 0x10);
}

//带颜色的puts
void color_puts(char *string, int color)
{
    set_text_color(color, 15);
    puts(string);
    set_text_color(0, 15);
}