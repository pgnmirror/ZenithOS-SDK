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

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>

#define LOG_INFO    0
#define LOG_WARNING 1
#define LOG_ERROR   2
#define LOG_FATAL   3

#define LOG_LEVEL LOG_INFO

#define prlog(level, fmt, ...) \
    do { \
        if (level >= LOG_LEVEL) { \
            const char* level_str = ""; \
            switch (level) { \
                case LOG_INFO:    level_str = "INFO"; break; \
                case LOG_WARNING: level_str = "WARNING"; break; \
                case LOG_ERROR:   level_str = "ERROR"; break; \
                case LOG_FATAL:   level_str = "FATAL"; break; \
            } \
            printf("[%s] %s:%d: " fmt "\n", level_str, __FILE__, __LINE__, ##__VA_ARGS__); \
        } \
    } while (0)

#define prlog_i(fmt, ...) prlog(LOG_INFO, fmt, ##__VA_ARGS__)
#define prlog_w(fmt, ...) prlog(LOG_WARNING, fmt, ##__VA_ARGS__)
#define prlog_e(fmt, ...) prlog(LOG_ERROR, fmt, ##__VA_ARGS__)
#define prlog_fe(fmt, ...) prlog(LOG_FATAL, fmt, ##__VA_ARGS__)

#endif // LOG_H

