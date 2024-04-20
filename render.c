#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// 渲染遊戲畫面
void Render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // 渲染食物
    SDL_Rect foodRect = {food.position.x, food.position.y, FOOD_SIZE, FOOD_SIZE};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &foodRect);

    // 渲染蛇
    for (int i = 0; i < snake.length; i++) {
        SDL_Rect snakeRect = {snake.body[i].x, snake.body[i].y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &snakeRect);
    }

    SDL_RenderPresent(renderer);
}

// 渲染遊戲結束畫面
void RenderGameOver(SDL_Renderer* renderer, TTF_Font* font, int score) {
    SDL_Color textColor = {255, 255, 255, 255};
    char scoreText[100];
    sprintf(scoreText, "Game Over\nScore: %d\nPress any key to exit", score);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int text_width = textSurface->w;
    int text_height = textSurface->h;
    SDL_FreeSurface(textSurface);

    SDL_Rect renderQuad = { (WINDOW_WIDTH - text_width) / 2, (WINDOW_HEIGHT - text_height) / 2, text_width, text_height };
    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
    SDL_DestroyTexture(textTexture);
}