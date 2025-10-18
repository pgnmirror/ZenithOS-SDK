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



#ifndef REPEAT_H
#define REPEAT_H

#include <unistd.h>
#include <stddef.h>
#include <errno.h>

static inline ssize_t printrep(const char *s, size_t n) {
    if (!s) { errno = EINVAL; return -1; }
    const char *p = s;
    size_t len = 0;
    while (p[len]) ++len;

    ssize_t total = 0;
    for (size_t i = 0; i < n; ++i) {
        size_t remaining = len;
        const char *cur = s;
        while (remaining > 0) {
            ssize_t w = write(STDOUT_FILENO, cur, remaining);
            if (w < 0) return -1; 
            total += w;
            remaining -= (size_t)w;
            cur += w;
        }
    }
    return total;
}

#define PRINTREP(s, n) (void)printrep((s), (size_t)(n))

#endif /* REPEAT_H */
