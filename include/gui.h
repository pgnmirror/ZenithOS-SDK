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

#ifndef GUI_H
#define GUI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SWITCHES 100
static int switch_count = 0;

typedef struct {
    int x, y, w, h;
    SDL_Color color_off, color_on;
    int is_on;
    int id;
} Switch;

typedef struct {
    int x, y, w, h;
    SDL_Color color;
} Rectangle;

typedef struct {
    int x, y, radius;
    SDL_Color color;
} Circle;

static Switch switches[MAX_SWITCHES];

void sdl_init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }
    if (TTF_Init() == -1) {
        printf("TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
        exit(1);
    }
}

void OnSwitch(int switch_id);

void addswitch(int switch_id, int x, int y, SDL_Color color_off, SDL_Color color_on) {
    if (switch_count < MAX_SWITCHES) {
        switches[switch_count].x = x;
        switches[switch_count].y = y;
        switches[switch_count].w = 100;
        switches[switch_count].h = 50;
        switches[switch_count].color_off = color_off;
        switches[switch_count].color_on = color_on;
        switches[switch_count].is_on = 0;
        switches[switch_count].id = switch_id;
        switch_count++;
    }
}

void draw_switch(SDL_Renderer *renderer, Switch *sw) {
    SDL_SetRenderDrawColor(renderer, sw->is_on ? sw->color_on.r : sw->color_off.r,
                           sw->is_on ? sw->color_on.g : sw->color_off.g,
                           sw->is_on ? sw->color_on.b : sw->color_off.b, 255);
    SDL_Rect rect = {sw->x, sw->y, sw->w, sw->h};
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

void handle_switch_click(SDL_Event *e, Switch *sw) {
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        if (x >= sw->x && x <= sw->x + sw->w && y >= sw->y && y <= sw->y + sw->h) {
            sw->is_on = !sw->is_on;
            printf("Switch %d is now %s\n", sw->id, sw->is_on ? "ON" : "OFF");
            if (sw->is_on) OnSwitch(sw->id);
        }
    }
}

void OnSwitch(int switch_id) {
    if (switch_id == 1) printf("Switch 1 activated\n");
    else if (switch_id == 2) printf("Switch 2 activated\n");
}

// Rectangles
void draw_rectangle(SDL_Renderer *renderer, Rectangle *rect) {
    SDL_SetRenderDrawColor(renderer, rect->color.r, rect->color.g, rect->color.b, rect->color.a);
    SDL_Rect sdl_rect = {rect->x, rect->y, rect->w, rect->h};
    SDL_RenderFillRect(renderer, &sdl_rect);
}

// Circles
void draw_circle(SDL_Renderer *renderer, Circle *circle) {
    SDL_SetRenderDrawColor(renderer, circle->color.r, circle->color.g, circle->color.b, circle->color.a);
    for (int w = 0; w < circle->radius * 2; w++) {
        for (int h = 0; h < circle->radius * 2; h++) {
            int dx = circle->radius - w;
            int dy = circle->radius - h;
            if ((dx*dx + dy*dy) <= (circle->radius*circle->radius)) {
                SDL_RenderDrawPoint(renderer, circle->x + dx, circle->y + dy);
            }
        }
    }
}

void printg(SDL_Renderer *renderer, int x, int y, const char *font_path, int font_size, const char *text, SDL_Color color) {
    TTF_Font *font = TTF_OpenFont(font_path, font_size);
    if (!font) {
        printf("Failed to load font %s: %s\n", font_path, TTF_GetError());
        return;
    }
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, text, color);
    if (!text_surface) {
        printf("Failed to create text surface: %s\n", TTF_GetError());
        TTF_CloseFont(font);
        return;
    }
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_Rect dst_rect = {x, y, text_surface->w, text_surface->h};
    SDL_RenderCopy(renderer, text_texture, NULL, &dst_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
    TTF_CloseFont(font);
}

#endif

