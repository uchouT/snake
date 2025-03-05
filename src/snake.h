#pragma once

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include <vector>
#include <set>
#include <algorithm>
#include <cstring>

using namespace std;
#define MAP_HEIGHT 20
#define MAP_WIDTH 40
#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

#define tail_UP 'i'
#define tail_DOWN 'k'
#define tail_LEFT 'j'
#define tail_RIGHT 'l'


typedef struct
{
    int x;
    int y;
} SnakeNode, Food;

typedef struct
{
    SnakeNode snakeNode[1000];
    int length;
    int speed;
} Snake;


void CursorGoto(int x, int y);
void HideCursor();
int Menu();
void help();
void about();
void InitMap();
void printFood();
int MoveSnake();
int isCorrect();
void SpeedControl();
void mode();
void ScoreRecord();
void LoadScore();
void InitBlock();