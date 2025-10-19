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

#ifndef RADIO_H
#define RADIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WIFI_INTERFACE "wlan0"

int radio_get_rssi() {
    FILE *fp = fopen("/proc/net/wireless", "r");
    if (!fp) {
        perror("Failed to open /proc/net/wireless");
        return -1;
    }

    char line[256];
    int rssi = -999;

    fgets(line, sizeof(line), fp);
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, WIFI_INTERFACE)) {
            int signal;
            if (sscanf(line, "%*s %*d %d", &signal) == 1) {
                rssi = signal;
            }
            break;
        }
    }

    fclose(fp);
    return rssi;
}

#endif // RADIO_H
