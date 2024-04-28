#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define SNAKE_BLOCK_SIZE 20
#define FOOD_SIZE 20
#define SNAKE_INITIAL_LENGTH 5
#define SNAKE_MAX_LENGTH 100

enum Direction { UP, DOWN, LEFT, RIGHT };

typedef struct {
    int x, y;
} Coordinate;

typedef struct {
    Coordinate body[SNAKE_MAX_LENGTH];
    int length;
    enum Direction dir;
} Snake;

typedef struct {
    Coordinate position;
    bool isEaten;
} Food;

Snake snake;
Food food;
int gameDelay = 100; // 設定遊戲延遲時間，控制遊戲速度
int score = 0;       // 遊戲分數

// 初始化蛇的位置和方向
void InitSnake() {
    snake.length = SNAKE_INITIAL_LENGTH;
    snake.dir = RIGHT;
    for (int i = 0; i < snake.length; i++) {
        snake.body[i].x = WINDOW_WIDTH / 2 - i * SNAKE_BLOCK_SIZE;
        snake.body[i].y = WINDOW_HEIGHT / 2;
    }
}

// 隨機生成食物的位置，確保不會與蛇的身體重疊
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

// 控制蛇的移動
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

// 檢查蛇是否撞到自己或邊界
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

// 渲染遊戲畫面
void Render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_Rect foodRect = {food.position.x, food.position.y, FOOD_SIZE, FOOD_SIZE};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &foodRect);

    for (int i = 0; i < snake.length; i++) {
        SDL_Rect snakeRect = {snake.body[i].x, snake.body[i].y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE};
        SDL_SetRenderDrawColor(renderer, 255 * i / snake.length, 255 - (255 * i / snake.length), (255 * i / snake.length), SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &snakeRect);
    }

    SDL_RenderPresent(renderer);
}

// 更新蛇的移動方向，根據玩家的鍵盤輸入
void UpdateDirection(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_UP:
                if (snake.dir != DOWN) snake.dir = UP;
                break;
            case SDLK_DOWN:
                if (snake.dir != UP) snake.dir = DOWN;
                break;
            case SDLK_LEFT:
                if (snake.dir != RIGHT) snake.dir = LEFT;
                break;
            case SDLK_RIGHT:
                if (snake.dir != LEFT) snake.dir = RIGHT;
                break;
        }
    }
}

// 主函數，初始化 SDL，創建視窗及渲染器，並進入遊戲主迴圈
int main(__attribute__((unused)) int argc, __attribute__((unused)) char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "無法初始化SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "無法初始化SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    window = SDL_CreateWindow("貪吃蛇", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "無法創建視窗: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "無法創建渲染器: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    InitSnake();
    GenerateFood();
    srand((unsigned int)time(NULL));

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else {
                UpdateDirection(&e);
            }
        }

        if (food.isEaten) {
            GenerateFood();
        }

        MoveSnake();
        if (CheckCollision()) {
            printf("遊戲結束\n分數: %d\n", score);
            break;
        }

        Render(renderer);
        SDL_Delay(gameDelay);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
