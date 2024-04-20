#include "game.h"
#include <SDL2/SDL.h>

// 更新蛇的移動方向
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