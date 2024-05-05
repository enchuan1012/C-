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

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point body[SNAKE_MAX_LENGTH];
    int length;
    Direction dir;
} Snake;

typedef struct {
    Point position;
    bool isEaten;
} Food;

static Snake snake;
static Food food;
static int gameDelay = 100;
static int score = 0;

void InitializeSnake() {
    snake.length = SNAKE_INITIAL_LENGTH;
    snake.dir = RIGHT;
    int center_x = WINDOW_WIDTH / 2;
    int center_y = WINDOW_HEIGHT / 2;
    for (int i = 0; i < snake.length; i++) {
        snake.body[i].x = center_x - i * SNAKE_BLOCK_SIZE;
        snake.body[i].y = center_y;
    }
}

void PlaceFood() {
    srand((unsigned)time(NULL));
    do {
        food.position.x = rand() % (WINDOW_WIDTH / FOOD_SIZE) * FOOD_SIZE;
        food.position.y = rand() % (WINDOW_HEIGHT / FOOD_SIZE) * FOOD_SIZE;
        food.isEaten = false;
        for (int i = 0; i < snake.length; i++) {
            if (food.position.x == snake.body[i].x && food.position.y == snake.body[i].y) {
                continue;
            }
        }
    } while (food.isEaten);
}

void ProcessMovement() {
    Point nextPosition = snake.body[0];
    switch (snake.dir) {
        case UP: nextPosition.y -= SNAKE_BLOCK_SIZE; break;
        case DOWN: nextPosition.y += SNAKE_BLOCK_SIZE; break;
        case LEFT: nextPosition.x -= SNAKE_BLOCK_SIZE; break;
        case RIGHT: nextPosition.x += SNAKE_BLOCK_SIZE; break;
    }

    if (nextPosition.x == food.position.x && nextPosition.y == food.position.y) {
        snake.length = (snake.length < SNAKE_MAX_LENGTH) ? snake.length + 1 : SNAKE_MAX_LENGTH;
        food.isEaten = true;
        score += 10;
    }

    for (int i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }
    snake.body[0] = nextPosition;
}

bool CheckForCollision() {
    Point head = snake.body[0];
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

void RenderGame(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_Rect foodRect = {food.position.x, food.position.y, FOOD_SIZE, FOOD_SIZE};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE); // 食物顏色保持鮮紅
    SDL_RenderFillRect(renderer, &foodRect);

    // 為蛇身使用更加動態的漸變色
    for (int i = 0; i < snake.length; i++) {
        float fraction = (float)i / snake.length;
        int r = (int)(255 * fraction);  // 紅色從深到淺
        int g = (int)(255 * (1 - fraction)); // 綠色從淺到深
        int b = (int)(128 + 127 * sin(fraction * 3.14159)); // 藍色使用正弦函數變化以增加視覺效果

        SDL_Rect snakeRect = {snake.body[i].x, snake.body[i].y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE};
        SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &snakeRect);
    }

    SDL_RenderPresent(renderer);
}
void UpdateDirectionFromInput(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_UP:    if (snake.dir != DOWN) snake.dir = UP; break;
            case SDLK_DOWN:  if (snake.dir != UP) snake.dir = DOWN; break;
            case SDLK_LEFT:  if (snake.dir != RIGHT) snake.dir = LEFT; break;
            case SDLK_RIGHT: if (snake.dir != LEFT) snake.dir = RIGHT; break;
        }
    }
}

int main() {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL初始化失敗: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "SDL_ttf初始化失敗: %s\n", TTF_GetError());
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

    InitializeSnake();
    PlaceFood();

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else {
                UpdateDirectionFromInput(&e);
            }
        }

        if (food.isEaten) {
            PlaceFood();
        }

        ProcessMovement();
        if (CheckForCollision()) {
            printf("遊戲結束\n分數: %d\n", score);
            break;
        }

        RenderGame(renderer);
        SDL_Delay(gameDelay);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
