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

#ifndef DEVICE_INFO_H
#define DEVICE_INFO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>

#define MAX_BUFFER_SIZE 256

// Get CPU architecture
const char* arch() {
    static char buffer[MAX_BUFFER_SIZE];
    FILE *file = fopen("/proc/cpuinfo", "r");
    if (file == NULL) {
        perror("Error opening /proc/cpuinfo");
        return NULL;
    }

    while (fgets(buffer, MAX_BUFFER_SIZE, file)) {
        if (strstr(buffer, "Architecture") != NULL) {
            fclose(file);
            return strchr(buffer, ':') + 2; // Skip "Architecture: "
        }
    }

    fclose(file);
    return NULL;
}

// Get RAM size
size_t ram() {
    FILE *file = fopen("/proc/meminfo", "r");
    if (file == NULL) {
        perror("Error opening /proc/meminfo");
        return 0;
    }

    char buffer[MAX_BUFFER_SIZE];
    size_t totalRam = 0;
    while (fgets(buffer, MAX_BUFFER_SIZE, file)) {
        if (strstr(buffer, "MemTotal") != NULL) {
            sscanf(buffer, "MemTotal: %zu kB", &totalRam);
            fclose(file);
            return totalRam / 1024;  // Convert to MB
        }
    }

    fclose(file);
    return 0;
}

// Get CPU information
const char* cpu() {
    static char buffer[MAX_BUFFER_SIZE];
    FILE *file = fopen("/proc/cpuinfo", "r");
    if (file == NULL) {
        perror("Error opening /proc/cpuinfo");
        return NULL;
    }

    while (fgets(buffer, MAX_BUFFER_SIZE, file)) {
        if (strstr(buffer, "model name") != NULL) {
            fclose(file);
            return strchr(buffer, ':') + 2; // Skip "model name: "
        }
    }

    fclose(file);
    return NULL;
}

// Get ROM size
size_t rom() {
    struct statvfs buf;
    if (statvfs("/", &buf) != 0) {
        perror("Error getting memory information");
        return 0;
    }

    return (buf.f_frsize * buf.f_blocks) / (1024 * 1024);  // In MB
}

// Print all device info
void sinfo() {
    printf("Architecture: %s\n", arch() ? arch() : "Failed to retrieve");
    printf("RAM: %zu MB\n", ram());
    printf("CPU: %s\n", cpu() ? cpu() : "Failed to retrieve");
    printf("ROM: %zu MB\n", rom());
}

#endif // DEVICE_INFO_H

