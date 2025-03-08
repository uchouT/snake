#include "snake.h"
#include <iostream> // 只是为了打印头像
using namespace std;

Snake snake;
Food food;
int foodScore;
char now_direction = RIGHT;
char direction = RIGHT;
char tail_direction = tail_RIGHT;
char head_direction = RIGHT;
int tmp_length;
char mode_code = '1';
char diff = '1';
const char *diff_name[3] = {"简单", "中等", "困难"};
const char *mode_name[3] = {"普通模式", "切尾巴", "多障碍"};
bool isMoveHead = (now_direction == UP || now_direction == DOWN || now_direction == LEFT || now_direction == RIGHT);

int tail_is_growing = 0;
int head_is_growing = 0;
// 创建障碍，采用 std::set，免去麻烦的去重步骤
struct Block
{
    int x, y;
    bool operator<(const Block &other) const
    {
        return (x < other.x || (x == other.x && y < other.y));
    }
};
set<Block> block;

int Menu()
{
    const char *current_mode = mode_name[mode_code - '1']; // 显示当前模式
    char current_diff[10] = {};
    if (mode_code == '3')
    {
        strcpy(current_diff, diff_name[diff - '1']);
    }
    LoadScore(); // 菜单展开前，载入分数
    CursorGoto(40, 12);
    printf("贪吃蛇，当前模式: %s %s", current_mode, current_diff);
    CursorGoto(40, 14);
    printf("1.开始游戏");
    CursorGoto(40, 16);
    printf("2.游戏帮助");
    CursorGoto(40, 18);
    printf("3.关于游戏");
    CursorGoto(40, 20);
    printf("4.选择游戏模式");
    CursorGoto(40, 22);
    printf("任意按键退出游戏");
    HideCursor();
    char ch = _getch();
    int result = ch - '0';
    system("cls");
    return result;
}

void InitMap()
{
    HideCursor();
    InitBlock();
    snake.snakeNode[0].x = MAP_WIDTH / 2 - 1;
    snake.snakeNode[0].y = MAP_HEIGHT / 2 - 1;
    CursorGoto(snake.snakeNode[0].x, snake.snakeNode[0].y);
    printf("O");
    snake.length = 3;
    snake.speed = 250;
    now_direction = RIGHT;

    for (int i = 1; i < snake.length; i++)
    {
        snake.snakeNode[i].x = snake.snakeNode[i - 1].x - 1;
        snake.snakeNode[i].y = snake.snakeNode[i - 1].y;
        CursorGoto(snake.snakeNode[i].x, snake.snakeNode[i].y);
        if (i == snake.length - 1)
        {
            printf("●"); // 初始化蛇尾
        }
        else
        {
            printf("O");
        }
    }

    for (int i = 1; i < MAP_HEIGHT - 1; i++)
    {
        CursorGoto(0, i);
        printf("|");
        CursorGoto(MAP_WIDTH - 1, i);
        printf("|");
    }
    for (int i = 1; i < MAP_WIDTH - 1; i++)
    {
        CursorGoto(i, 0);
        printf("-");
        CursorGoto(i, MAP_HEIGHT - 1);
        printf("-");
    }

    printFood();

    CursorGoto(50, 5);
    printf("当前得分: 0");
}

void about()
{
    cout << R"(⠀⠀⠀⣀⣀⣀⣀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣀⣀⠀⠀⠀⠀⠀
⠀⢠⠏⠁⠀⢸⠏⠉⠉⠛⠒⠦⣤⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⡤⠴⠖⠚⠛⠋⠉⠹⣇⠀⠉⢙⠆⠀⠀⠀
⠀⠸⡄⠀⠀⣸⠀⠀⠀⠀⠀⠀⠀⠀⠉⠙⠲⣦⣶⣛⣛⣉⣹⣦⣶⣚⣉⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⣿⡀⠀⢀⡇⠀⠀⠀
⠀⠀⠳⡄⠀⣿⠀⠀⠀⠀⠀⠀⢀⣠⡤⠖⠚⠛⠉⠉⠀⠀⠀⠀⠈⠉⠉⠛⠛⠶⢤⣄⠀⠀⠀⠀⠀⢸⡇⠀⡜⠀⠀⠀⠀
⠀⠀⠀⠹⣤⡏⠀⠀⠀⠀⣠⡶⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠛⢦⡀⠀⠀⠈⡇⢸⡀⠀⠀⠀⠀
⠀⠀⠀⣠⢿⡇⠀⠀⢀⡶⠋⠀⠀⠀⠀⠀⠀⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣦⠀⠀⢷⠀⠹⣆⠀⠀⠀
⠀⣰⠞⠁⢸⡇⠀⢰⣟⠁⠀⠀⠀⠀⠀⠀⣼⡇⠀⠀⠀⠀⠀⠀⢠⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣇⠀⢸⡀⠀⠈⢧⡀⠀
⢰⡁⠀⠀⢸⠀⢀⡟⠀⠀⠀⠀⠀⠀⠀⣰⠃⣇⠀⠀⠀⠀⠀⠀⢸⡎⢳⣄⠀⠀⠀⠀⠀⠀⠀⠀⢹⡆⠸⡇⠀⠀⠀⡇⠀
⠀⠣⣄⣀⣼⠀⣸⠇⠀⠀⠀⠀⠈⠉⣹⠏⠉⢿⡉⠀⠀⠀⠀⠀⠈⣏⠉⠙⣯⡉⠉⠉⠀⠀⠀⠀⠀⢿⣤⣷⣤⡴⠚⠁⠀
⠀⠀⣠⠟⠉⠉⣻⠀⠀⠀⠀⠀⠀⢰⠏⠀⠤⠬⢷⡄⠀⠀⠀⠀⠀⡿⠤⠤⠈⠻⢦⣀⠀⠀⠀⠀⠀⢸⡇⠀⠈⢿⣄⠀⠀
⠀⣼⠏⠀⠀⠀⠸⡇⠀⠀⣤⣤⣴⣯⣤⣤⣤⣤⣦⣬⠳⢤⣄⣠⣾⣷⣾⣿⣿⣿⣿⣿⠿⢷⣄⡀⠀⢸⠇⠀⠀⠀⠛⣧⡀
⠐⠿⣦⣀⠀⠀⠀⣻⣄⠀⢉⡿⠛⡟⠉⢹⣿⣿⣿⣿⡄⠀⠀⠉⣿⠀⣸⣿⣿⣿⣿⣿⣄⣀⠈⡟⠳⣿⠀⠀⠀⢀⣀⣨⡷
⠀⠀⠀⠈⠉⠛⢚⡏⠙⢶⣏⣀⣤⣷⣴⣿⣿⣿⣿⣿⠇⠀⠀⠀⠸⣿⣿⣿⣿⣿⣿⣿⡏⠉⠉⡇⠀⠹⡏⠉⠉⠉⠀⠀⠀
⠀⠀⠀⠀⠀⠀⣸⠃⠀⠀⣏⠁⠀⢿⡿⣿⣿⣿⣿⠟⠀⠀⠀⠀⠀⠙⠿⠿⠿⠿⠋⣼⠁⠀⠀⡇⠀⠀⢻⡀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⣿⠀⠀⠀⢻⡀⠀⢈⣷⣄⠈⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣞⣡⣠⣤⣼⡃⠀⠀⠈⣧⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠻⠤⠤⢴⣺⡗⠚⠋⠹⡿⡙⠲⠤⣀⣀⣈⣋⣁⣀⡠⠤⢖⣿⠓⣿⠀⠀⠀⣿⣉⣓⠦⠤⠿⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⣠⡖⠋⠉⠀⢿⡄⠀⠀⡇⡟⢦⡀⠀⢠⣾⣉⣣⡀⠀⣠⠞⢻⠀⣿⠀⠀⣼⠃⠀⢸⠙⢦⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⣸⠁⢱⡀⠀⠀⠈⠳⣄⣠⣇⡇⠀⠉⢙⡿⠀⠀⠈⣿⠛⠃⠀⢺⡦⣼⣆⡴⠃⠀⠀⡾⠀⢸⡇⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠘⢦⣀⢳⠀⠀⣀⡤⠞⠋⠁⢻⣄⢀⣼⠁⠀⠀⠀⢸⡄⠀⢀⣾⠁⠀⠈⠉⠛⠒⢦⣧⣤⣾⠁⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⢈⡭⠷⡏⠉⠀⠀⠀⠀⠀⠙⠳⠿⠶⣶⣶⣿⡭⠥⠶⠟⠁⠀⠀⠀⠀⠀⢀⡾⠁⠀⢈⡇⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⢸⡀⠀⠙⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⠏⠀⠀⢠⡞⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠻⣄⠀⠈⠻⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣰⠟⠁⠀⠀⣴⠏⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠳⣄⠀⠀⠙⠲⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⡴⠛⠁⠀⢀⡴⠛⠁⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢦⣀⠀⠀⠉⠛⠲⠦⠤⠤⠤⠴⠶⠚⠋⠁⠀⣀⣤⣾⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣷⣦⣄⣀⠀⠀⠀⠀⠀⠀⣀⣀⣤⣶⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣿⣿⣿⣿⣿⣿⡿⠋⠉⠉⠹⣿⣿⣿⣿⣿⣿⣿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⠿⠿⠿⠟⠋⠀⠀⠀⠀⠀⠈⠛⠻⠟⠛⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀)";
    CursorGoto(50, 12);
    printf("贪吃蛇游戏");
    CursorGoto(50, 14);
    printf("作者：uchouT");
    CursorGoto(50, 16);
    printf("按任意键返回主菜单");
    _getch();
    system("cls");
    CursorGoto(40, 12);

}

void help()
{
    CursorGoto(40, 12);
    printf("控制蛇头 @: w 上 s 下 a 左 d 右");
    CursorGoto(40, 13);
    printf("控制蛇尾 ●: i 上 k 下 j 左 l 右");
    CursorGoto(40, 15);
    printf("普通模式和障碍模式，撞到蛇身死亡；");
    CursorGoto(40, 16);
    printf("切尾巴模式中，蛇头撞身体将尾巴切断，蛇尾撞身体死亡");
    CursorGoto(40, 18);
    printf("按任意键返回主菜单");
    _getch();
    system("cls");
}

void CursorGoto(int x, int y)
{
    HANDLE hout;
    COORD cor;
    hout = GetStdHandle(STD_OUTPUT_HANDLE);
    cor.X = x;
    cor.Y = y;
    SetConsoleCursorPosition(hout, cor);
}

void HideCursor()
{
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursor_info = {1, 0};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void printFood()
{
    int flag = 1;
    while (flag)
    {
        food.x = rand() % (MAP_WIDTH - 2) + 1;
        food.y = rand() % (MAP_HEIGHT - 2) + 1;
        for (int i = 0; i < snake.length; i++)
        {
            if (food.x == snake.snakeNode[i].x && food.y == snake.snakeNode[i].y)
            {
                break;
            }
            if (i == snake.length - 1)
            {
                flag = 0;
            }
        }
    }
    CursorGoto(food.x, food.y);

    // 打印不同的食物。概率为 5:3:2
    int tmp = rand() % 10;
    if (tmp <= 4)
    {
        foodScore = 1;
        printf("★");
    }
    else if (tmp <= 7)
    {
        foodScore = 2;
        printf("❀");
    }
    else
    {
        foodScore = 3;
        printf("✪");
    }
    return;
}

int isCorrect()
{

    if (isMoveHead)
    {
        if (snake.snakeNode[0].x == 0 || snake.snakeNode[0].x == MAP_WIDTH - 1 || snake.snakeNode[0].y == 0 || snake.snakeNode[0].y == MAP_HEIGHT - 1)
        {
            return 0;
        };
        if (mode_code == '1' || mode_code == '3')
        {
            for (int i = 1; i < snake.length; i++) // 普通模式和障碍模式，撞到蛇身死亡
            {
                if (snake.snakeNode[0].x == snake.snakeNode[i].x && snake.snakeNode[0].y == snake.snakeNode[i].y)
                {
                    return 0;
                }
            }
        }
    }
    else
    {
        // 尾巴撞墙立即死亡
        if (snake.snakeNode[snake.length - 1].x == 0 || snake.snakeNode[snake.length - 1].x == MAP_WIDTH - 1 || snake.snakeNode[snake.length - 1].y == 0 || snake.snakeNode[snake.length - 1].y == MAP_HEIGHT - 1)
        {
            return 0;
        }

        for (int i = 0; i < snake.length - 1; i++) // 尾巴撞到蛇身必死
        {
            if (snake.snakeNode[snake.length - 1].x == snake.snakeNode[i].x && snake.snakeNode[snake.length - 1].y == snake.snakeNode[i].y)
            {
                return 0;
            }
        }
    }

    // 障碍模式，碰到方块死亡
    if (mode_code == '3')
    {
        for (auto &i : block)
        {
            if ((snake.snakeNode[0].x == i.x && snake.snakeNode[0].y == i.y) || (snake.snakeNode[snake.length - 1].x == i.x && snake.snakeNode[snake.length - 1].y == i.y))
            {
                return 0;
            }
        }
    }

    return 1;
}

int MoveSnake()
{

    // Todo: 增加双头蛇

    // 判断蛇头蛇尾是否正在生长
    if (head_is_growing > 0)
    {
        head_is_growing--;
    }
    if (tail_is_growing > 0)
    {
        tail_is_growing--;
    }
    int flag = 0;
    SnakeNode head = snake.snakeNode[0];
    // 确定蛇尾方向
    SnakeNode tail = snake.snakeNode[snake.length - 1];

    if (tail.x != snake.snakeNode[snake.length - 2].x)
    {
        if (tail.x < snake.snakeNode[snake.length - 2].x)
        {
            tail_direction = tail_RIGHT;
        }
        else
        {
            tail_direction = tail_LEFT;
        }
    }
    else
    {
        if (tail.y < snake.snakeNode[snake.length - 2].y)
        {
            tail_direction = tail_DOWN;
        }
        else
        {
            tail_direction = tail_UP;
        }
    }

    if (head.x != snake.snakeNode[1].x)
    {
        if (head.x < snake.snakeNode[1].x)
        {
            head_direction = RIGHT;
        }
        else
        {
            head_direction = LEFT;
        }
    }
    else
    {
        if (head.y < snake.snakeNode[1].y)
        {
            head_direction = DOWN;
        }
        else
        {
            head_direction = UP;
        }
    }

    if (_kbhit())
    {
        direction = _getch();
        switch (direction)
        {
        case UP:
            if (now_direction != DOWN && head_direction != UP && !head_is_growing)
            {
                now_direction = UP;
            }
            break;
        case DOWN:
            if (now_direction != UP && head_direction != DOWN && !head_is_growing)
            {
                now_direction = DOWN;
            }
            break;
        case LEFT:
            if (now_direction != RIGHT && head_direction != LEFT && !head_is_growing)
            {
                now_direction = LEFT;
            }
            break;
        case RIGHT:
            if (now_direction != LEFT && head_direction != RIGHT && !head_is_growing)
            {
                now_direction = RIGHT;
            }
            break;

        // 增加尾巴控制
        case tail_UP:
            if (now_direction != tail_DOWN && tail_direction != tail_UP && !tail_is_growing)
            {
                now_direction = tail_UP;
            }
            break;
        case tail_DOWN:
            if (now_direction != tail_UP && tail_direction != tail_DOWN && !tail_is_growing)
            {
                now_direction = tail_DOWN;
            }
            break;
        case tail_LEFT:
            if (now_direction != tail_RIGHT && tail_direction != tail_LEFT && !tail_is_growing)
            {
                now_direction = tail_LEFT;
            }
            break;
        case tail_RIGHT:
            if (now_direction != tail_LEFT && tail_direction != tail_RIGHT && !tail_is_growing)
            {
                now_direction = tail_RIGHT;
            }
            break;
        }
    }
    isMoveHead = (now_direction == UP || now_direction == DOWN || now_direction == LEFT || now_direction == RIGHT);
    // 蛇头移动
    if (isMoveHead)
    {
        for (int i = snake.length - 1; i > 0; i--)
        {
            snake.snakeNode[i] = snake.snakeNode[i - 1];
        }
        CursorGoto(snake.snakeNode[1].x, snake.snakeNode[1].y);
        printf("O");
    }
    else // 蛇尾移动
    {
        for (int i = 0; i < snake.length - 1; i++)
        {
            snake.snakeNode[i] = snake.snakeNode[i + 1];
        }
        CursorGoto(snake.snakeNode[snake.length - 2].x, snake.snakeNode[snake.length - 2].y);
        printf("O");
    }

    // 实现双头蛇移动
    switch (now_direction)
    {
    case UP:
        snake.snakeNode[0].y--;
        break;
    case DOWN:
        snake.snakeNode[0].y++;
        break;
    case LEFT:
        snake.snakeNode[0].x--;
        break;
    case RIGHT:
        snake.snakeNode[0].x++;
        break;
    case tail_UP:
        snake.snakeNode[snake.length - 1].y--;
        break;
    case tail_DOWN:
        snake.snakeNode[snake.length - 1].y++;
        break;
    case tail_LEFT:
        snake.snakeNode[snake.length - 1].x--;
        break;
    case tail_RIGHT:
        snake.snakeNode[snake.length - 1].x++;
        break;
    }

    // 蛇头移动
    if (isMoveHead)
    {
        if (snake.snakeNode[0].x == food.x && snake.snakeNode[0].y == food.y)
        {
            snake.length += foodScore;
            tail_is_growing += foodScore;
            flag = 1;
            snake.snakeNode[snake.length - foodScore] = tail;
        };

        if (!flag)
        {
            if (!tail_is_growing)
            {
                CursorGoto(tail.x, tail.y);
                printf(" ");
            }
        }
        else
        {
            printFood();
            CursorGoto(50, 5);
            printf("当前得分: %3d", snake.length - 3);
        }
    }

    // 移动蛇尾
    else
    {
        if (snake.snakeNode[snake.length - 1].x == food.x && snake.snakeNode[snake.length - 1].y == food.y)
        {
            snake.length += foodScore;
            head_is_growing += foodScore;
            flag = 1;
            for (int i = snake.length - 1; i >= foodScore; i--)
            {
                snake.snakeNode[i] = snake.snakeNode[i - foodScore];
            }
            snake.snakeNode[foodScore - 1] = head;
        };

        if (!flag)
        {
            if (!head_is_growing)
            {
                CursorGoto(head.x, head.y);
                printf(" ");
            }
        }
        else
        {
            printFood();
            CursorGoto(50, 5);
            printf("当前得分: %3d", snake.length - 3);
        }
    }
    CursorGoto(snake.snakeNode[0].x, snake.snakeNode[0].y);
    printf("@");
    if (!tail_is_growing)
    {
        CursorGoto(snake.snakeNode[snake.length - 1].x, snake.snakeNode[snake.length - 1].y);
        printf("●");
    }

    // 添加模式 2：撞蛇身不死，截断尾巴
    if (mode_code == '2')
    {
        if (isMoveHead)
        {
            for (int i = 1; i < snake.length; i++)
            {
                if (snake.snakeNode[0].x == snake.snakeNode[i].x && snake.snakeNode[0].y == snake.snakeNode[i].y)
                {
                    // 清除尾部，并刷新分数

                    for (int j = i + 1; j < snake.length; j++)
                    {
                        CursorGoto(snake.snakeNode[j].x, snake.snakeNode[j].y);
                        printf(" ");
                    }
                    snake.length = i;
                    CursorGoto(50, 5);
                    printf("当前得分: %3d", snake.length - 3);
                }
            }
        };
    }

    if (!isCorrect())
    {
        system("cls");
        CursorGoto(40, 14);
        printf("最终得分：%d", snake.length - 3);
        CursorGoto(40, 16);
        printf("游戏结束");
        CursorGoto(40, 18);
        printf("按任意键退出游戏");
        _getch();
        system("cls");
        ScoreRecord();
        return 0;
    }
    SpeedControl();
    Sleep(snake.speed);
    return 1;
}

void SpeedControl()
{
    switch (snake.length)
    {
    case 6:
        snake.speed = 200;
        break;
    case 9:
        snake.speed = 180;
        break;
    case 12:
        snake.speed = 160;
        break;
    case 15:
        snake.speed = 140;
        break;
    case 18:
        snake.speed = 120;
        break;
    case 21:
        snake.speed = 100;
        break;
    case 24:
        snake.speed = 80;
        break;
    case 27:
        snake.speed = 60;
        break;
    case 30:
        snake.speed = 40;
        break;
    default:
        break;
    }
}

// 记录分数
void ScoreRecord()
{
    CursorGoto(40, 15);
    printf("是否记录成绩? y/n");
    char ch;
    while (ch = _getch())
    {
        if (ch == 'y' || ch == 'n')
        {
            break;
        }
    }
    if (ch == 'y')
    {
        int score = snake.length - 3;
        char player[50];
        CursorGoto(40, 18);
        printf("输入名称：");
        fgets(player, sizeof(player), stdin);
        if (player[0] == '\n')
        {
            strcpy(player, "unkown_player");
        }
        player[strcspn(player, "\n")] = '\0';
        FILE *fp = nullptr;
        switch (mode_code)
        {
        case '1':
            fp = fopen("NormalRank.txt", "a");
            break;
        case '2':
            fp = fopen("TailRank.txt", "a");
            break;
        case '3':
            fp = fopen("ObstacleRank.txt", "a");
            break;
        default:
            break;
        }
        if (fp == nullptr)
        {
            return;
        }
        fprintf(fp, "%s %d\n", player, score);
        fclose(fp);
    }
    system("cls");
    return;
}

// 载入分数到数组中
void LoadScore()
{
    // 定义玩家结构，记录游玩数据
    struct Player
    {
        int Score;
        char name[50];
    };
    vector<Player> playerlist;
    FILE *fp = nullptr;
    switch (mode_code)
    {
    case '1':
        fp = fopen("NormalRank.txt", "r");
        break;
    case '2':
        fp = fopen("TailRank.txt", "r");
        break;
    case '3':
        fp = fopen("ObstacleRank.txt", "r");
        break;
    default:
        break;
    }
    if (fp == nullptr)
    {
        return; // 没有数据时，直接返回，不显示排行榜
    }
    char tmp[100];
    while (fgets(tmp, sizeof(tmp), fp))
    {
        Player tmp_player;
        sscanf(tmp, "%s %d", tmp_player.name, &tmp_player.Score);
        playerlist.push_back(tmp_player);
    }
    fclose(fp);
    sort(playerlist.begin(), playerlist.end(), [](auto &x, auto &y)
         { return x.Score > y.Score; }); // 根据 Score 属性进行排序
    CursorGoto(80, 10);
    printf("Player \t Score");
    for (int i = 0; i < playerlist.size() && i < 10; i++) // 显示排行榜在 menu 界面，最多显示 10 个
    {
        CursorGoto(80, 10 + (i + 1) * 2);
        printf("%s \t %d", playerlist.at(i).name, playerlist.at(i).Score);
    }
    return;
}

// 增加模式选择：
void mode()
{
    CursorGoto(40, 12);
    printf("请选择游戏模式");
    CursorGoto(40, 14);
    printf("1.%s", mode_name[0]);
    CursorGoto(40, 16);
    printf("2.%s", mode_name[1]);
    CursorGoto(40, 18);
    printf("3.%s", mode_name[2]);
    CursorGoto(40, 20);
    printf("任意键返回主菜单");
    HideCursor();
    char tmp_code = _getch();
    char tmp_diff;
    switch (tmp_code)
    {
    case '1':
    case '2':
        mode_code = tmp_code;
        break;
    case '3':
        mode_code = tmp_code;
        system("cls");
        CursorGoto(40, 12);
        printf("选择障碍数量");
        CursorGoto(40, 14);
        printf("1.简单");
        CursorGoto(40, 16);
        printf("2.中等");
        CursorGoto(40, 18);
        printf("3.困难");
        CursorGoto(40, 20);
        printf("任意键返回主菜单");
        HideCursor();
        tmp_diff = _getch();
        switch (tmp_diff)
        {
        case '1':
        case '2':
        case '3':
            diff = tmp_diff;
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }

    system("cls");
    return;
}

void InitBlock()
{
    if (mode_code != '3') // 非障碍模式直接退出
    {
        return;
    }
    int block_num;
    switch (diff)
    {
    case '1':
        block_num = 5;
        break;
    case '2':
        block_num = 10;
        break;
    case '3':
        block_num = 20;
        break;
    default:
        break;
    }

    while (block.size() <= block_num)
    {
        Block block_tmp;
        block_tmp.x = rand() % (MAP_WIDTH - 2) + 1;
        block_tmp.y = rand() % (MAP_HEIGHT - 2) + 1;
        block.insert(block_tmp);
    }
    for (auto &i : block)
    {
        CursorGoto(i.x, i.y);
        printf("■");
    }
    return;
}