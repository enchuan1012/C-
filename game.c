#include "game.h"
#include <stdlib.h>

// 蛇和食物的全局實例
Snake snake;
Food food;
int gameDelay;
int score = 0;

// 初始化蛇身
void InitSnake() {
    snake.length = SNAKE_INITIAL_LENGTH;
    snake.dir = RIGHT;
    for (int i = 0; i < snake.length; i++) {
        snake.body[i].x = WINDOW_WIDTH / 2 - i * SNAKE_BLOCK_SIZE;
        snake.body[i].y = WINDOW_HEIGHT / 2;
    }
}

// 生成食物的位置
void GenerateFood() {
    bool collision;
    do {
        collision = false;
        food.position.x = rand() % ((WINDOW_WIDTH - FOOD_SIZE) / FOOD_SIZE) * FOOD_SIZE;
        food.position.y = rand() % ((WINDOW_HEIGHT - FOOD_SIZE) / FOOD_SIZE) * FOOD_SIZE;
        for (int i = 0; i < snake.length; i++) {
            if (food.position.x == snake.body[i].x && food.position.y == snake.body[i].y) {
                collision = true;
                break;
            }
        }
    } while (collision);
    food.isEaten = false;
}

// 移動蛇身
void MoveSnake() {
    Coordinate nextPosition = snake.body[0];
    switch (snake.dir) {
        case UP: nextPosition.y -= SNAKE_BLOCK_SIZE; break;
        case DOWN: nextPosition.y += SNAKE_BLOCK_SIZE; break;
        case LEFT: nextPosition.x -= SNAKE_BLOCK_SIZE; break;
        case RIGHT: nextPosition.x += SNAKE_BLOCK_SIZE; break;
    }

    if (nextPosition.x == food.position.x && nextPosition.y == food.position.y) {
        snake.length++;
        if (snake.length > SNAKE_MAX_LENGTH) snake.length = SNAKE_MAX_LENGTH;
        food.isEaten = true;
        score += 10;
    }

    for (int i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }
    snake.body[0] = nextPosition;
}

// 檢查是否發生碰撞
bool CheckCollision() {
    Coordinate head = snake.body[0];
    if (head.x < 0 || head.x >= WINDOW_WIDTH || head.y < 0 || head.y >= WINDOW_HEIGHT) {
        return true;
    }
    for (int i = 1; i < snake.length; i++) {
        if (head.x == snake.body[i].x && head.y == snake.body[i].y) {
            return true;
        }
    }
    return false;
}