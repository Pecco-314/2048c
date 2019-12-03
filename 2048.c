#include "2048.h"

int main()
{
    Init(MAXX, MAXY, BLOCK_SIZE);
    generate();
    generate();
    //set_block_value(0, 0, 1024);
    //MessageBox(NULL, "��Ӯ�ˣ�", "2048", 0);
    while (1)
        ;
    return 0;
}

// ��ʼ����x�У�y�У����Ӵ�СΪbs
void Init(int x, int y, int bs)
{
    initUI(x, y, bs);
    init_map(x, y);
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
COORD get_coord(block *b)
{
    COORD coord;
    coord.X = 1 + (BLOCK_SIZE * 2 + 1) * b->y + (BLOCK_SIZE * 2 - cnt_digits(b->value) + 1) / 2;
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
void init_map(int x, int y)
{
    for (int i = 0; i < x; ++i)
        for (int j = 0; j < y; ++j)
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

// ��ʼ�����棬����Ϊx��y��
void initUI(int x, int y, int bs)
{
    window_resize(x, y);
    system("title 2048"); //���ñ���
    system("color F0");   //���ñ���ɫΪ��ɫ��ǰ��ɫΪ��ɫ
    puts("2048����̨�� - made by Pecco\n");
    puts("W - ����  A - ����  S - ����  D - ����");
    print_board(x, y, bs);
}

// ����x��y�е����̣����ô��ڴ�С
void window_resize(int x, int y)
{
    char s[30] = "";
    int lines = 7 + x * (BLOCK_SIZE + 1);
    int cols = max(40, 3 + y * (BLOCK_SIZE * 2 + 1)); //cols����С��40
    sprintf(s, "mode con cols=%d lines=%d", cols, lines);
    system(s);
}

//��ӡ����n�У�����Ϊy�����Ӵ�СΪbs���ɶ��Ƹ��ָ�ʽ
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

//��ӡ����ͷn�У�����Ϊy�����Ӵ�СΪbs
void print_board_head(int y, int n, int bs)
{
    print_board_line("��", "��", "��", "��", y, n, bs);
}

//��ӡ������n�У�����Ϊy�����Ӵ�СΪbs
void print_board_body(int y, int n, int bs)
{
    print_board_line("��", " ", "��", "��", y, n, bs);
}

//��ӡ���̽��洦n�У�����Ϊy�����Ӵ�СΪbs
void print_board_crossing(int y, int n, int bs)
{
    print_board_line("��", "��", "��", "��", y, n, bs);
}

//��ӡ����βn�У�����Ϊy�����Ӵ�СΪbs
void print_board_tail(int y, int n, int bs)
{
    print_board_line("��", "��", "��", "��", y, n, bs);
}

//��ӡx��y�е����̣����Ӵ�СΪbs
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
