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

#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>

static int audio_initialized = 0;

void audio_init() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize audio! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        exit(1);
    }

    audio_initialized = 1;
}

void playaudio(const char *filename) {
    if (!audio_initialized) {
        audio_init();
    }

    Mix_Music *music = Mix_LoadMUS(filename);
    if (!music) {
        printf("Failed to load audio file %s! SDL_mixer Error: %s\n", filename, Mix_GetError());
        return;
    }

    if (Mix_PlayMusic(music, 1) == -1) {
        printf("Failed to play music! SDL_mixer Error: %s\n", Mix_GetError());
    }


    Mix_FreeMusic(music);
}

void audio_quit() {
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

#endif
