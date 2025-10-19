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
 
#ifndef TIMER_H
#define TIMER_H

#include <unistd.h>   // sleep, usleep
#include <stdint.h>   // uint32_t

static inline void wait(uint32_t sec) {
    sleep(sec);
}

static inline void uwait(uint32_t ms) {
    usleep(ms * 1000);
}

#endif // TIMER_H
