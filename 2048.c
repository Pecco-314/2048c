#include "2048.h"

int main()
{
    Init();
    while (1)
        Step();
    return 0;
}

// ��ʼ��
void Init()
{
    initUI();
    init_map();
    new_game();
}

// ����ÿһ���Ĳ���
void Step()
{
    prepare_to_input();
    char ch = getch();
    if (ch == -32)
        ch = getch();
    //printf("%d", ch);///
    //TODO:�ƶ����ϲ�
    judge();
    if (has_space())
        generate();
}

//�ѹ���Ƶ�������
void prepare_to_input()
{
    COORD coord = {0, (BLOCK_SIZE + 1) * MAXX + 4};
    locate(coord);
}

//�ж���Ϸʤ�����
void judge()
{
    if (has_space())
        return;
    else if (judge_win())
        win();
    else if (judge_lose())
        lose();
}

//�ж��Ƿ�ʤ����ʤ������1
int judge_win()
{
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            if (map[i][j]->value == GOAL)
                return 1;
    return 0;
}

//�ж��Ƿ�ʧ�ܣ�ʧ�ܷ���1
int judge_lose()
{
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            if ((i < MAXX - 1 && map[i + 1][j]->value == map[i][j]->value) ||
                (j < MAXY - 1 && map[i][j + 1]->value == map[i][j]->value))
                return 0;
    return 1;
}

//��Ϸʧ�ܴ���
void lose()
{
    puts("���޿��ƶ��ķ��飬��Ϸʧ�ܡ�\n N - ����Ϸ  C - ������Ϸ");
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

//��Ϸʤ������
void win()
{
    puts("��Ϸʤ������ϲ��\n N - ����Ϸ  C - ������Ϸ");
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

//���������
void empty_input_area()
{
    prepare_to_input();
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < max(40, 3 + MAXY * (BLOCK_SIZE * 2 + 1)); ++j)
            putchar(' ');
    prepare_to_input();
}

// ����Ϸ��������̣����������������
void new_game()
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            set_block_value(i, j, 0);
    generate();
    generate();
}

// ����һ��ʮ����������λ��
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

// ���ù��λ��
void locate(COORD coord)
{
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// ���뷽��ָ�룬����ʵ�ʵĹ��λ��
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

//�����·���
block *new_block(int x, int y)
{
    block *b = (block *)malloc(sizeof(block));
    b->x = x;
    b->y = y;
    b->value = 0;
    return b;
}

//��ѯ���޿�λ
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

//�������2��4
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

//��ʼ����Ϸ��ͼ����
void init_map()
{
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            map[i][j] = new_block(i, j);
}

// ����ĳ����ķ����ֵ������ӡ
int set_block_value(int i, int j, int value)
{
    map[i][j]->value = value;
    print_block(i, j);
}

// ��ӡĳ����ķ��飬��������ֵ��0����ոø�
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

// ��ʼ�����棬����Ϊx��y��
void initUI()
{
    window_resize();
    window_fix();
    system("title 2048"); //���ñ���
    system("color F0");   //���ñ���ɫΪ��ɫ��ǰ��ɫΪ��ɫ
    puts("2048����̨�� - made by Pecco\n");
    puts("W - ����  A - ����  S - ����  D - ����");
    print_board();
}

// ����x��y�е����̣����ô��ڴ�С
void window_resize()
{
    char s[30] = "";
    int lines = 8 + MAXX * (BLOCK_SIZE + 1);
    int cols = max(40, 3 + MAXY * (BLOCK_SIZE * 2 + 1)); //cols����С��40
    sprintf(s, "mode con cols=%d lines=%d", cols, lines);
    system(s);
}

//ʹ���ڴ�С���ɸı�
void window_fix()
{
    SetWindowLongPtrA(
        GetConsoleWindow(),
        GWL_STYLE,
        GetWindowLongPtrA(GetConsoleWindow(), GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX);
}

//��ӡ����n�У�����Ϊy���ɶ��Ƹ��ָ�ʽ
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

//��ӡ����ͷn�У�����Ϊy
void print_board_head(int y, int n)
{
    print_board_line("��", "��", "��", "��", y, n);
}

//��ӡ������n�У�����Ϊy
void print_board_body(int y, int n)
{
    print_board_line("��", " ", "��", "��", y, n);
}

//��ӡ���̽��洦n�У�����Ϊy
void print_board_crossing(int y, int n)
{
    print_board_line("��", "��", "��", "��", y, n);
}

//��ӡ����βn�У�����Ϊy
void print_board_tail(int y, int n)
{
    print_board_line("��", "��", "��", "��", y, n);
}

//��ӡ����
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
