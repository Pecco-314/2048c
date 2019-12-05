#include "2048.h"

// ��ʼ��
void Init()
{
    init_config();
    initUI();
    init_map();
    load_game();
    init_hist();
}

// ����ÿһ���Ĳ���
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

//������Ϸ
void save_game()
{
    system("attrib -r save.dat"); //ʹ�浵�ļ���д
    FILE *save = fopen("save.dat", "w");
    fprintf(save, "%d %d ", MAXX, MAXY); //�洢��ǰ���̵Ĵ�С
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            fprintf(save, "%d ", Map[i][j]->value);
    fprintf(save, "%llu %llu", Pts, Bests);
    fclose(save);
    system("attrib +r save.dat"); //���ô浵�ļ�ֻ��
}

//��ȡ��Ϸ
void load_game()
{
    FILE *save = fopen("save.dat", "r");
    if (save != NULL)
    {
        int x, y;
        fscanf(save, "%d%d", &x, &y); //��ȡ���̴�С
        for (int i = 0; i < MAXX; ++i)
            for (int j = 0; j < MAXY; ++j)
                fscanf(save, "%d", &Map[i][j]->value);
        fscanf(save, "%llu%llu", &Pts, &Bests); //��ȡ����
        fclose(save);
        while (x != MAXX || y != MAXY) //������̴�С��ƥ�䣬����һ������
            trigger_warning(UNMATCH_SAVE_FORMAT, x, y);
        reprint_all();
        save_game();
    }
    else
        new_game();
}

//������ʷ
void save_history()
{
    system("attrib -r -h hist.dat"); //ȡ����ʷ�ļ���ֻ��������
    FILE *hist = fopen("hist.dat", "a");
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            fprintf(hist, "%d ", Map[i][j]->value);
    fprintf(hist, "%llu %llu\n", Pts, Bests);
    fclose(hist);
    system("attrib +r +h hist.dat"); //������ʷ�ļ�Ϊֻ��������
}

//��ʼ����ʷ�ļ���������ֻ���������ԭ��������ɾ��
void init_hist()
{
    system("attrib -r -h hist.dat"); //ȡ��ԭ�����ڵ���ʷ�ļ���ֻ��������
    FILE *hist = fopen("hist.dat", "w");
    fclose(hist);
    save_history();
    system("attrib +r +h hist.dat"); //������ʷ�ļ�Ϊ������ֻ��
}

//��hist�ļ������һ�����µ�ǰ����
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

//����һ��
void withdraw_step()
{
    system("attrib -r -h hist.dat");
    delete_last_line("hist.dat");
    update_from_last_line();
    system("attrib +r +h hist.dat");
}

//��ȡһ���ļ�������
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

//ɾ��ĳ���ļ������һ��
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

//�������봦���ƶ�
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
    else if (ch == 1063) //F5������Ϸ
        new_game();
    else if (ch == 1059) //F1������
        WinExec("notepad.exe help.txt", SW_SHOW);
    else if (ch == 'C' || ch == 'c') // C������
        open_config();
    else if (ch == 'Q' || ch == 'q') // Q���˳�
        exit(0);
    else if (ch == 'R' || ch == 'r') // R�����³�ʼ��
        Init();
    else if (ch == 'Z' || ch == 'z') // Z������
    {
        withdraw_step();
        return; //��������ʷ
    }
    if (c && all_move(c))
        generate();
    save_history();
}

//�������ļ�
void open_config()
{
    char ch;
    WinExec("notepad.exe .config", SW_SHOW);
    empty_input_area();
    prepare_to_input();
    color_puts("�����ļ������Ѹı䣬�Ƿ����³�ʼ����Ϸ��\n Y - ��  N - ��", 1);
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

//��ȡ������ĳ�������Ͻ����ŵķ��飬����߽緵��NULL
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

//�õ�������ĳ�������ƶ�Ӧ�õ����λ��
block *get_end(block *bk, char dir)
{
    block *b = bk;
    while (get_neighbor(b, dir) != NULL && get_neighbor(b, dir)->value == 0)
        b = get_neighbor(b, dir);
    return b;
}

//������ͼ��ĳ�������ϵ����ڷ����ϣ���ϳɹ�����1�����򷵻�0
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

//������ĳ�������ƶ�������ͼ��ϣ��ƶ����ϳɹ�����1�����򷵻�0
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

//�������ϱ������з���
int forall_d(int procedure(block *, char))
{
    int sec = 0;
    for (int i = MAXX - 1; i >= 0; --i)
        for (int j = 0; j < MAXY; ++j)
            sec = max(sec, procedure(Map[i][j], 'd'));
    return sec;
}

//�������±������з���
int forall_u(int procedure(block *, char))
{
    int sec = 0;
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            sec = max(sec, procedure(Map[i][j], 'u'));
    return sec;
}

//�������ұ������з���
int forall_l(int procedure(block *, char))
{
    int sec = 0;
    for (int j = 0; j < MAXY; ++j)
        for (int i = MAXX - 1; i >= 0; --i)
            sec = max(sec, procedure(Map[i][j], 'l'));
    return sec;
}

//��������������з���
int forall_r(int procedure(block *, char))
{
    int sec = 0;
    for (int j = MAXY - 1; j >= 0; --j)
        for (int i = MAXX - 1; i >= 0; --i)
            sec = max(sec, procedure(Map[i][j], 'r'));
    return sec;
}

// ȫ����ĳ�������ƶ������ƶ�����ϳɹ��򷵻�1
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

//�ѹ���Ƶ�������
void prepare_to_input()
{
    COORD coord = {0, (BLOCK_SIZE + 1) * MAXX + 6};
    locate(coord);
}

//�ж���Ϸʤ�����
void judge()
{
    if (judge_win())
        win();
    else if (judge_lose())
        lose();
}

//�ж��Ƿ�ʤ����ʤ������1
int judge_win()
{
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            if (Map[i][j]->value == GOAL)
                return 1;
    return 0;
}

//�ж��Ƿ�ʧ�ܣ�ʧ�ܷ���1
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

//��Ϸʧ�ܴ���
void lose()
{
    prepare_to_input();
    color_puts("���޿��ƶ��ķ��飬��Ϸʧ�ܡ�\n N - ����Ϸ  C - ������Ϸ\a", 12);
    new_or_continue();
}

//��Ϸʤ������
void win()
{
    prepare_to_input();
    color_puts("��Ϸʤ������ϲ��\n N - ����Ϸ  C - ������Ϸ", 3);
    new_or_continue();
    char c;
}

//�û�ѡ������Ϸ�������Ϸ
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

//ʹ���з���ɽ��
void all_combinable()
{
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            Map[i][j]->combinable = 1;
}

//���������
void empty_input_area()
{
    prepare_to_input();
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < Cols; ++j)
            putchar(' ');
    prepare_to_input();
}

// ����Ϸ��������̣���������������飬��������Ϊ0
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

// ����һ��ʮ����������λ��
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

// ���ù��λ��
void locate(COORD coord)
{
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// ���뷽��ָ�룬����ʵ�ʵĹ��λ��
COORD get_coord(const block *b)
{
    COORD coord;
    coord.X = 1 + (BLOCK_SIZE * 2 + 1) * b->y;
    coord.Y = 4 + (BLOCK_SIZE + 1) * b->x + BLOCK_SIZE / 2;
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

// ���´�ӡ���з��飬�����·�������߷�
void reprint_all()
{
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            print_block(i, j);
    update_pts();
}

// ���·�������߷�
void update_pts()
{
    COORD coord = {7, (BLOCK_SIZE + 1) * MAXX + 4};
    locate(coord); //��λ����ӡ������λ��
    printf("%-*llu", Cols - 7, Pts);
    coord.Y += 1;
    locate(coord);
    printf("%-*llu", Cols - 7, Bests);
}

//�������2��4
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

//��ʼ������
void init_config()
{
    FILE *config = fopen(".config", "r");
    fscanf(config, "%*s%*s%*s%d%*s%d%*s%d%*s%d%*s %c", &MAXX, &MAXY, &BLOCK_SIZE, &GOAL, &AUTO_SAVE);
    //�ֶ�����ע�ͣ�����д��̫�ã����Ҳ���д���ӵ��ַ�������
    fclose(config);
}

//��ʼ����Ϸ��ͼ����
void init_map()
{
    Map = (block ***)malloc(sizeof(block **) * MAXX);
    for (int i = 0; i < MAXX; ++i)
        Map[i] = (block **)malloc(sizeof(block *) * MAXY);
    for (int i = 0; i < MAXX; ++i)
        for (int j = 0; j < MAXY; ++j)
            Map[i][j] = new_block(i, j);
}

// ����ĳ����ķ����ֵ
void set_block_value(int i, int j, int value)
{
    Map[i][j]->value = value;
}

// ��ӡĳ����ķ��飬��������ֵ��0����ոø�
void print_block(int i, int j)
{
    block *b = Map[i][j];
    COORD coord = get_coord(b);
    locate(coord);
    for (int i = 0; i < BLOCK_SIZE * 2; ++i)
        putchar(' ');
    if (Map[i][j]->value)
    {
        coord.X += (BLOCK_SIZE * 2 - count_digits(b->value) + 1) / 2; //���ж���
        locate(coord);
        printf("%d", b->value);
    }
}

// ��ʼ�����棬����Ϊx��y��
void initUI()
{
    window_resize();
    window_fix();
    hide_cursor();
    system("title 2048"); //���ñ���
    system("color F0");   //���ñ���ɫΪ��ɫ��ǰ��ɫΪ��ɫ
    puts("2048����̨�� - made by Pecco\n");
    puts("����������ʱ���κ����⣬���԰�F1�򿪰����ļ�");
    print_board();
}

// ���ݸ��ӵĶ��ٺʹ�С���ô��ڴ�С
void window_resize()
{
    char s[30] = "";
    Lines = 9 + MAXX * (BLOCK_SIZE + 1);
    Cols = max(46, 3 + MAXY * (BLOCK_SIZE * 2 + 1)); // Cols����С��46
    sprintf(s, "mode con cols=%d lines=%d", Cols, Lines);
    system(s);
}

//ʹ���ڴ�С���ɸı�
void window_fix()
{
    SetWindowLongPtrA(
        GetConsoleWindow(), GWL_STYLE,
        GetWindowLongPtrA(GetConsoleWindow(), GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX);
}

//���ع��
void hide_cursor()
{
    CONSOLE_CURSOR_INFO cursor;
    cursor.bVisible = FALSE;
    cursor.dwSize = sizeof(cursor);
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorInfo(handle, &cursor);
}

//��ӡ����n�У�����Ϊy���ɶ��Ƹ��ָ�ʽ
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
    puts("������");
    puts("��ߣ�");
}

//�������棬v1��v2�ǿ�ѡ�������Ҫ��Ϊ0
void trigger_warning(enum warning w, int v1, int v2)
{
    switch (w)
    {
    case UNMATCH_SAVE_FORMAT:
        warn_unmatch_save_format(v1, v2);
    }
}

//�浵��ʽ��ƥ�侯��
void warn_unmatch_save_format(int x, int y)
{
    prepare_to_input();
    set_text_color(4);
    printf("���棺�浵�����̴�С�����ò�ͳһ\n"
           "�浵Ϊ��%d��%d��  ����Ϊ��%d��%d��\n"
           "N - ����Ϸ  C - �޸�����  Q - �˳�",
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

//����������ɫ
// 0-�� 1-�� 2-�� 3-ǳ�� 4-�� 5-�� 6-�� 7-�� 8-�� 9-����
// 10-���� 11-��ǳ�� 12-���� 13-���� 14-���� 15-����
void set_text_color(int color)
{
    HANDLE winHandle; //���
    winHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(winHandle, color + 240);
}

//����ɫ��puts
// 0-�� 1-�� 2-�� 3-ǳ�� 4-�� 5-�� 6-�� 7-�� 8-�� 9-����
// 10-���� 11-��ǳ�� 12-���� 13-���� 14-���� 15-����
void color_puts(char *string, int color)
{
    set_text_color(color);
    puts(string);
    set_text_color(0);
}