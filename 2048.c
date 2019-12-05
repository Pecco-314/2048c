#include "2048.h"

// 初始化
void Init()
{
    init_config();
    initUI();
    init_map();
    load_game();
    init_hist();
}

// 处理每一步的操作
void Step()
{
    prepare_to_input();
    int ch = getch();
    if (ch == 224 || ch == 0)
        ch = getch() + 1000;
    process_input(ch);
    reprint_all();
    if (AUTO_SAVE == 'Y')
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
            fprintf(save, "%d ", Map[i][j]->value);
    fprintf(save, "%llu %llu", Pts, Bests);
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
        fscanf(save, "%d%d", &x, &y); //读取棋盘大小
        for (int i = 0; i < MAXX; ++i)
            for (int j = 0; j < MAXY; ++j)
                fscanf(save, "%d", &Map[i][j]->value);
        fscanf(save, "%llu%llu", &Pts, &Bests); //读取分数
        fclose(save);
        while (x != MAXX || y != MAXY) //如果棋盘大小不匹配，触发一个警告
            trigger_warning(UNMATCH_SAVE_FORMAT, x, y);
        reprint_all();
        save_game();
    }
    else
        new_game();
}

//储存历史
void save_history()
{
    system("attrib -r -h hist.dat"); //取消历史文件的只读和隐藏
    FILE *hist = fopen("hist.dat", "a");
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            fprintf(hist, "%d ", Map[i][j]->value);
    fprintf(hist, "%llu %llu\n", Pts, Bests);
    fclose(hist);
    system("attrib +r +h hist.dat"); //设置历史文件为只读且隐藏
}

//初始化历史文件（隐藏且只读），如果原来存在则删除
void init_hist()
{
    system("attrib -r -h hist.dat"); //取消原来存在的历史文件的只读和隐藏
    FILE *hist = fopen("hist.dat", "w");
    fclose(hist);
    save_history();
    system("attrib +r +h hist.dat"); //设置历史文件为隐藏且只读
}

//用hist文件的最后一步更新当前棋盘
void update_from_last_line()
{
    char buffer[1000];
    int cntlines = count_lines("hist.dat");
    FILE *hist = fopen("hist.dat", "r");
    for (int i = 0; i < cntlines - 1; ++i)
        fgets(buffer, sizeof(buffer), hist);
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            fscanf(hist, "%d", &Map[i][j]->value);
    fscanf(hist, "%llu%llu", &Pts, &Bests);
    reprint_all();
    fclose(hist);
}

//撤回一步
void withdraw_step()
{
    system("attrib -r -h hist.dat");
    delete_last_line("hist.dat");
    update_from_last_line();
    system("attrib +r +h hist.dat");
}

//获取一个文件的行数
int count_lines(char *filename)
{
    int cnt = 0;
    char ch;
    FILE *file = fopen(filename, "r");
    while ((ch = fgetc(file)) != EOF)
        if (ch == '\n')
            cnt++;
    fclose(file);
    return cnt;
}

//删除某个文件的最后一行
void delete_last_line(char *filename)
{
    char buffer[1000];
    int cntlines = count_lines(filename);
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.dat", "w");
    for (int i = 0; i < cntlines - 1; ++i)
    {
        fgets(buffer, sizeof(buffer), file);
        fputs(buffer, temp);
    }
    fclose(file);
    fclose(temp);
    remove(filename);
    rename("temp.dat", filename);
}

//根据输入处理移动
void process_input(int ch)
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
    else if (ch == 1063) //F5：新游戏
        new_game();
    else if (ch == 1059) //F1：帮助
        WinExec("notepad.exe help.txt", SW_SHOW);
    else if (ch == 'C' || ch == 'c') // C：配置
        open_config();
    else if (ch == 'Q' || ch == 'q') // Q：退出
        exit(0);
    else if (ch == 'R' || ch == 'r') // R：重新初始化
        Init();
    else if (ch == 'Z' || ch == 'z') // Z：撤回
    {
        withdraw_step();
        return; //不保存历史
    }
    if (c && all_move(c))
        generate();
    save_history();
}

//打开配置文件
void open_config()
{
    char ch;
    WinExec("notepad.exe .config", SW_SHOW);
    empty_input_area();
    prepare_to_input();
    color_puts("配置文件可能已改变，是否重新初始化游戏？\n Y - 是  N - 不", 1);
    while (ch = getch())
    {
        if (ch == 'Y' || ch == 'y')
            Init();
        if (ch == 'Y' || ch == 'y' || ch == 'N' || ch == 'n')
            break;
    }
    empty_input_area();
    prepare_to_input();
}

//获取方块在某个方向上紧挨着的方块，若达边界返回NULL
block *get_neighbor(const block *b, char dir)
{
    switch (dir)
    {
    case 'l':
        return b->y == 0 ? NULL : Map[b->x][b->y - 1];
    case 'r':
        return b->y == MAXY - 1 ? NULL : Map[b->x][b->y + 1];
    case 'u':
        return b->x == 0 ? NULL : Map[b->x - 1][b->y];
    case 'd':
        return b->x == MAXX - 1 ? NULL : Map[b->x + 1][b->y];
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
        Pts += end->value;
        Bests = max(Bests, Pts);
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
            sec = max(sec, procedure(Map[i][j], 'd'));
    return sec;
}

//从上往下遍历所有方块
int forall_u(int procedure(block *, char))
{
    int sec = 0;
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            sec = max(sec, procedure(Map[i][j], 'u'));
    return sec;
}

//从左往右遍历所有方块
int forall_l(int procedure(block *, char))
{
    int sec = 0;
    for (int j = 0; j < MAXY; ++j)
        for (int i = MAXX - 1; i >= 0; --i)
            sec = max(sec, procedure(Map[i][j], 'l'));
    return sec;
}

//从右往左遍历所有方块
int forall_r(int procedure(block *, char))
{
    int sec = 0;
    for (int j = MAXY - 1; j >= 0; --j)
        for (int i = MAXX - 1; i >= 0; --i)
            sec = max(sec, procedure(Map[i][j], 'r'));
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
    if (judge_win())
        win();
    else if (judge_lose())
        lose();
}

//判断是否胜利，胜利返回1
int judge_win()
{
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            if (Map[i][j]->value == GOAL)
                return 1;
    return 0;
}

//判断是否失败，失败返回1
int judge_lose()
{
    if (has_space())
        return 0;
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            if ((i < MAXX - 1 && Map[i + 1][j]->value == Map[i][j]->value) || (j < MAXY - 1 && Map[i][j + 1]->value == Map[i][j]->value))
                return 0;
    return 1;
}

//游戏失败处理
void lose()
{
    prepare_to_input();
    color_puts("已无可移动的方块，游戏失败。\n N - 新游戏  C - 继续游戏\a", 12);
    new_or_continue();
}

//游戏胜利处理
void win()
{
    prepare_to_input();
    color_puts("游戏胜利，恭喜！\n N - 新游戏  C - 继续游戏", 3);
    new_or_continue();
    char c;
}

//用户选择新游戏或继续游戏
void new_or_continue()
{
    char c;
    while (c = getch())
    {
        if (c == 'N' || c == 'n')
            new_game();
        if (c == 'c' || c == 'C' || c == 'N' || c == 'n')
        {
            empty_input_area();
            break;
        }
    }
}

//使所有方块可结合
void all_combinable()
{
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            Map[i][j]->combinable = 1;
}

//清空输入区
void empty_input_area()
{
    prepare_to_input();
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < Cols; ++j)
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
    Pts = 0;
    reprint_all();
}

// 返回一个十进制整数的位数
int count_digits(int n)
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
            if (Map[i][j]->value == 0)
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
    printf("%-*llu", Cols - 7, Pts);
    coord.Y += 1;
    locate(coord);
    printf("%-*llu", Cols - 7, Bests);
}

//随机生成2或4
void generate()
{
    srand(time(NULL) * clock());
    int v = rand() % ROF ? 2 : 4;
    while (1)
    {
        int x = rand() % MAXX, y = rand() % MAXY;
        if (Map[x][y]->value == 0)
        {
            set_block_value(x, y, v);
            break;
        }
    }
}

//初始化配置
void init_config()
{
    FILE *config = fopen(".config", "r");
    fscanf(config, "%*s%*s%*s%d%*s%d%*s%d%*s%d%*s %c", &MAXX, &MAXY, &BLOCK_SIZE, &GOAL, &AUTO_SAVE);
    //手动跳过注释，这样写不太好，但我不想写复杂的字符串处理
    fclose(config);
}

//初始化游戏地图数组
void init_map()
{
    Map = (block ***)malloc(sizeof(block **) * MAXX);
    for (int i = 0; i < MAXX; ++i)
        Map[i] = (block **)malloc(sizeof(block *) * MAXY);
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            Map[i][j] = new_block(i, j);
}

// 设置某坐标的方块的值
void set_block_value(int i, int j, int value)
{
    Map[i][j]->value = value;
}

// 打印某坐标的方块，如果方块的值是0则清空该格
void print_block(int i, int j)
{
    block *b = Map[i][j];
    COORD coord = get_coord(b);
    locate(coord);
    for (int i = 0; i < BLOCK_SIZE * 2; ++i)
        putchar(' ');
    if (Map[i][j]->value)
    {
        coord.X += (BLOCK_SIZE * 2 - count_digits(b->value) + 1) / 2; //居中对齐
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
    Lines = 9 + MAXX * (BLOCK_SIZE + 1);
    Cols = max(46, 3 + MAXY * (BLOCK_SIZE * 2 + 1)); // Cols不能小于46
    sprintf(s, "mode con cols=%d lines=%d", Cols, Lines);
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

//触发警告，v1和v2是可选项，若不需要则为0
void trigger_warning(enum warning w, int v1, int v2)
{
    switch (w)
    {
    case UNMATCH_SAVE_FORMAT:
        warn_unmatch_save_format(v1, v2);
    }
}

//存档格式不匹配警告
void warn_unmatch_save_format(int x, int y)
{
    prepare_to_input();
    set_text_color(4);
    printf("警告：存档的棋盘大小与配置不统一\n"
           "存档为：%d行%d列  配置为：%d行%d列\n"
           "N - 新游戏  C - 修改配置  Q - 退出",
           x, y, MAXX, MAXY);
    set_text_color(0);
    char c;
    while ((c = getch()) != 'n' && c != 'N')
    {
        if (c == 'c' || c == 'C')
        {
            open_config();
            return;
        }
        if (c == 'q' || c == 'Q')
            exit(0);
    }
    new_game();
    empty_input_area();
}

//设置文字颜色
// 0-黑 1-蓝 2-绿 3-浅蓝 4-红 5-紫 6-黄 7-白 8-灰 9-淡蓝
// 10-淡绿 11-淡浅绿 12-淡红 13-淡紫 14-淡黄 15-亮白
void set_text_color(int color)
{
    HANDLE winHandle; //句柄
    winHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(winHandle, color + 240);
}

//带颜色的puts
// 0-黑 1-蓝 2-绿 3-浅蓝 4-红 5-紫 6-黄 7-白 8-灰 9-淡蓝
// 10-淡绿 11-淡浅绿 12-淡红 13-淡紫 14-淡黄 15-亮白
void color_puts(char *string, int color)
{
    set_text_color(color);
    puts(string);
    set_text_color(0);
}