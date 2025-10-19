/*
 * ZenithOS SDK - ICMPv2 Header File
 * New ICMP Remake!
 *
 * Copyright (C) 2025 ne5link
 *
 * Licensed under the GNU General Public License v3.0 (GPLv3).
 * See <https://www.gnu.org/licenses/> for details.
 *
 * Made by ne5link <3
 */

// WARNING! This include is using very low-level network access. It usually requires root to use the include.

#ifndef ICMP_H
#define ICMP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>

#define PURPLE "\033[1;35m"
#define RESET  "\033[0m"

// icmp types
#define ICMP_ECHO_REQUEST 8  // req
#define ICMP_ECHO_REPLY   0  // reply

#define ICMP_HEADER_LEN 8
#define ICMP_DATA_LEN 56

struct icmp_packet {
    struct icmphdr header;
    char data[ICMP_DATA_LEN];
};

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char*)buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int send_icmp_request(const char *target_ip) {
    int sockfd;
    struct sockaddr_in dest_addr;
    struct icmp_packet packet;
    
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror(PURPLE "[ICMPv2]" RESET " Socket creation failed");
        return -1;
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(target_ip);

    memset(&packet, 0, sizeof(packet));
    packet.header.type = ICMP_ECHO_REQUEST;
    packet.header.un.echo.id = getpid() & 0xFFFF;
    packet.header.un.echo.sequence = 1;

    snprintf(packet.data, ICMP_DATA_LEN, "Test");
    packet.header.checksum = checksum(&packet, sizeof(packet));

    printf(PURPLE "[ICMPv2]" RESET " Sending Echo Request to %s...\n", target_ip);

    if (sendto(sockfd, &packet, sizeof(packet), 0,
        (struct sockaddr*)&dest_addr, sizeof(dest_addr)) <= 0) {
        perror(PURPLE "[ICMPv2]" RESET " Sendto failed");
        close(sockfd);
        return -1;
    }

    printf(PURPLE "[ICMPv2]" RESET " Echo Request sent successfully 🚀\n");

    close(sockfd);
    return 0;
}

char* get_hostname_from_ip(const char *ip) {
    struct hostent *host;
    struct in_addr addr;
    static char hostname[256];

    inet_pton(AF_INET, ip, &addr);
    host = gethostbyaddr(&addr, sizeof(addr), AF_INET);

    if (host == NULL) {
        fprintf(stderr, PURPLE "[ICMPv2]" RESET " Could not resolve hostname!\n");
        return NULL;
    }

    strncpy(hostname, host->h_name, sizeof(hostname) - 1);
    return hostname;
}

#endif // ICMP_H

