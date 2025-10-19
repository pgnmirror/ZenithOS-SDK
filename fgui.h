/*
 * ZenithOS SDK - Header File
 *
 * Copyright (C) 2025 ne5link
 *
 * Licensed under the GNU General Public License v3.0 (GPLv3).
 * See <https://www.gnu.org/licenses/> for details.
 *
 * Made by ne5link <3
 */

#ifndef FGUI_H
#define FGUI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

typedef struct {
    Uint8 r, g, b, a;
} FColor;

typedef enum {
    BAR_TOP,
    BAR_BOTTOM
} FBarPosition;

typedef struct {
    SDL_Rect rect;
    FColor color;
    char text[128];
    TTF_Font* font;
} FBar;

static void InitBar(FBar* bar, int width, int height, FColor color, FBarPosition position, int window_height) {
    if (!bar) return;
    bar->rect.w = width;
    bar->rect.h = height;
    bar->color = color;
    bar->text[0] = '\0';
    bar->font = NULL;

    if (position == BAR_TOP) {
        bar->rect.x = 0;
        bar->rect.y = 0;
    } else {
        bar->rect.x = 0;
        bar->rect.y = window_height - height;
    }
}

static void RenderBar(SDL_Renderer* renderer, FBar* bar) {
    if (!renderer || !bar) return;

    SDL_SetRenderDrawColor(renderer, bar->color.r, bar->color.g, bar->color.b, bar->color.a);
    SDL_RenderFillRect(renderer, &bar->rect);

    if (bar->text[0] != '\0' && bar->font) {
        SDL_Color sdlColor = {255, 255, 255, 255}; // текст белый
        SDL_Surface* surf = TTF_RenderText_Blended(bar->font, bar->text, sdlColor);
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_Rect dst = {bar->rect.x + 10, bar->rect.y + (bar->rect.h - surf->h)/2, surf->w, surf->h};
        SDL_RenderCopy(renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(surf);
    }
}

static void SetBarText(FBar* bar, const char* text, TTF_Font* font) {
    if (!bar || !text || !font) return;
    strncpy(bar->text, text, sizeof(bar->text)-1);
    bar->text[sizeof(bar->text)-1] = '\0';
    bar->font = font;
}

#endif // FGUI_H

