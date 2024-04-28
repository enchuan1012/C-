#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 定義視窗寬度和高度
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
// 定義蛇身和食物的大小
#define SNAKE_BLOCK_SIZE 20
#define FOOD_SIZE 20
// 定義初始蛇身長度和最大長度
#define SNAKE_INITIAL_LENGTH 5
#define SNAKE_MAX_LENGTH 100

// 定義蛇移動方向的列舉型別
enum Direction { UP, DOWN, LEFT, RIGHT };

// 定義座標結構
typedef struct {
    int x, y;
} Coordinate;

// 定義蛇身結構
typedef struct {
    Coordinate body[SNAKE_MAX_LENGTH];
    int length;
    enum Direction dir;
} Snake;

// 定義食物結構
typedef struct {
    Coordinate position;
    bool isEaten;
} Food;

// 蛇身和食物的全域變數
Snake snake;
Food food;
// 遊戲延遲時間
int gameDelay;
// 分數
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

// 生成食物
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

    // 如果蛇頭碰到食物,增加蛇身長度及分數
    if (nextPosition.x == food.position.x && nextPosition.y == food.position.y) {
        snake.length++;
        if (snake.length > SNAKE_MAX_LENGTH) snake.length = SNAKE_MAX_LENGTH;
        food.isEaten = true;
        score += 10;
    }

    // 更新蛇身位置
    for (int i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }
    snake.body[0] = nextPosition;
}

// 檢查碰撞
bool CheckCollision() {
    Coordinate head = snake.body[0];
    // 檢查是否撞到牆壁
    if (head.x < 0 || head.x >= WINDOW_WIDTH || head.y < 0 || head.y >= WINDOW_HEIGHT) {
        return true;
    }
    // 檢查是否撞到自己身體
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

    // 渲染食物
    SDL_Rect foodRect = {food.position.x, food.position.y, FOOD_SIZE, FOOD_SIZE};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &foodRect);

    // 渲染蛇身
    for (int i = 0; i < snake.length; i++) {
        int red = 255 * i / snake.length;
        int green = 255 - (255 * i / snake.length);
        int blue = (255 * i / snake.length);
        SDL_Rect snakeRect = {snake.body[i].x, snake.body[i].y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE};
        SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &snakeRect);
    }

    SDL_RenderPresent(renderer);
}

// 更新移動方向
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

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    // 選擇遊戲難度
    int difficulty;
    printf("請選擇遊戲難度:\n1. 簡單模式\n2. 普通模式\n3. 困難模式\n選擇: ");
    scanf("%d", &difficulty);

    // 設定遊戲延遲時間
    switch (difficulty) {
        case 1:
            gameDelay = 150;
            break;
        case 2:
            gameDelay = 100;
            break;
        case 3:
            gameDelay = 75;
            break;
        default:
            gameDelay = 100;
            break;
    }

    // 初始化 SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "無法初始化SDL: %s\n", SDL_GetError());
        return 1;
    }

    // 初始化 SDL_ttf
    if (TTF_Init() < 0) {
        fprintf(stderr, "無法初始化SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // 創建視窗
    window = SDL_CreateWindow("貪吃蛇", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
       fprintf(stderr, "無法創見視窗: %s\n", SDL_GetError());
       SDL_Quit();
       return 1;
   }

   // 創建渲染器
   renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
   if (!renderer) {
       fprintf(stderr, "無法創建渲染器: %s\n", SDL_GetError());
       SDL_DestroyWindow(window);
       SDL_Quit();
       return 1;
   }

   // 初始化蛇身和生成食物
   InitSnake();
   GenerateFood();
   srand((unsigned int)time(NULL));

   // 遊戲主循環
   bool quit = false;
   SDL_Event e;
   while (!quit) {
       // 處理事件
       while (SDL_PollEvent(&e) != 0) {
           if (e.type == SDL_QUIT) {
               quit = true;
           } else {
               UpdateDirection(&e);
           }
       }

       // 如果食物被吃,生成新的食物
       if (food.isEaten) {
           GenerateFood();
       }

       // 移動蛇身
       MoveSnake();
       // 檢查碰撞
       if (CheckCollision()) {
           printf("遊戲結束\n分數: %d\n", score);
           break;
       }

       // 渲染遊戲畫面
       Render(renderer);
       SDL_Delay(gameDelay);
   }

   // 清理並退出
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   SDL_Quit();
   return 0;
}
