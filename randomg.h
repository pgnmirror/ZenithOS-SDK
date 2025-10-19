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

#ifndef RANDOMG_H
#define RANDOMG_H

#include <stdlib.h>
#include <time.h>

static inline void rg_init() {
    static int initialized = 0;
    if (!initialized) {
        srand((unsigned int)time(NULL));
        initialized = 1;
    }
}

static inline int rg_rand_int(int min, int max) {
    rg_init();
    return min + rand() % (max - min + 1);
}

static inline float rg_rand_float(float min, float max) {
    rg_init();
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

typedef struct { unsigned char r, g, b; } rg_color;
static inline rg_color rg_rand_color() {
    rg_init();
    rg_color c = { (unsigned char)rg_rand_int(0,255),
                   (unsigned char)rg_rand_int(0,255),
                   (unsigned char)rg_rand_int(0,255) };
    return c;
}

#define rg_choice(arr, size) (arr[rg_rand_int(0, size-1)])

#endif 

