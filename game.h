#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define SNAKE_BLOCK_SIZE 20
#define FOOD_SIZE 20
#define SNAKE_INITIAL_LENGTH 5
#define SNAKE_MAX_LENGTH 100

// 方向枚舉
enum Direction { UP, DOWN, LEFT, RIGHT };

// 坐標結構
typedef struct {
    int x, y;
} Coordinate;

// 蛇結構
typedef struct {
    Coordinate body[SNAKE_MAX_LENGTH];
    int length;
    enum Direction dir;
} Snake;

// 食物結構
typedef struct {
    Coordinate position;
    bool isEaten;
} Food;

extern Snake snake;
extern Food food;
extern int gameDelay;
extern int score;

void InitSnake();
void GenerateFood();
void MoveSnake();
bool CheckCollision();

#endif