#include "snake.h"

int main(void)
{
    SetConsoleOutputCP(65001);
    srand((unsigned)time(NULL));
    int end = 1, result;
    while (end)
    {
        result = Menu();
        switch (result)
        {
        case 1:
            InitMap();
            while (MoveSnake())
                ;
            break;
        case 2:
            help();
            break;
        case 3:
            about();
            break;
        case 4:
            mode();
            break;
        default:
            end = 0;
            break;
        }
    }
}