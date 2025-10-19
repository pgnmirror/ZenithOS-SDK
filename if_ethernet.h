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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/ethtool.h>
#include <linux/if_packet.h>
#include <linux/sockios.h>

int zeth_get_mac_address(int sockfd, const char *ifname, unsigned char *mac) {
    struct ifreq ifr;

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == -1) {
        perror("Failed to get MAC address");
        return -1;
    }

    memcpy(mac, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
    return 0;
}

int zeth_set_mac_address(int sockfd, const char *ifname, const unsigned char *mac) {
    struct ifreq ifr;

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    memcpy(ifr.ifr_hwaddr.sa_data, mac, ETH_ALEN);

    if (ioctl(sockfd, SIOCSIFHWADDR, &ifr) == -1) {
        perror("Failed to set MAC address");
        return -1;
    }

    return 0;
}

int zeth_get_link_speed(int sockfd, const char *ifname) {
    struct ethtool_cmd edata;
    struct ifreq ifr;

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ifr.ifr_data = (char *)&edata;

    if (ioctl(sockfd, SIOCETHTOOL, &ifr) == -1) {
        perror("Failed to get link speed");
        return -1;
    }

    return edata.speed;
}

int zeth_set_link_speed(int sockfd, const char *ifname, int speed) {
    struct ethtool_cmd edata;
    struct ifreq ifr;

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ifr.ifr_data = (char *)&edata;

    edata.speed = speed;

    if (ioctl(sockfd, SIOCETHTOOL, &ifr) == -1) {
        perror("Failed to set link speed");
        return -1;
    }

    return 0;
}
