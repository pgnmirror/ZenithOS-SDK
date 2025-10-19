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


#ifndef NETUTILS_H
#define NETUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#define PURPLE "\033[1;35m"
#define RESET  "\033[0m"

static inline int is_valid_ip(const char *ip) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
    if(result == 1) {
        printf(PURPLE "[NETUTILS]" RESET " IP %s is valid!\n", ip);
        return 1;
    } else {
        printf(PURPLE "[NETUTILS]" RESET " IP %s is invalid!\n", ip);
        return 0;
    }
}

static inline char* hostname_to_ip(const char *hostname, char *ip_buffer, size_t buf_len) {
    struct hostent *he;
    struct in_addr **addr_list;

    if((he = gethostbyname(hostname)) == NULL) {
        perror(PURPLE "[NETUTILS]" RESET " Failed to resolve hostname");
        return NULL;
    }

    addr_list = (struct in_addr **)he->h_addr_list;
    if(addr_list[0] != NULL) {
        strncpy(ip_buffer, inet_ntoa(*addr_list[0]), buf_len);
        ip_buffer[buf_len-1] = '\0';
        printf(PURPLE "[NETUTILS]" RESET " %s resolved to %s\n", hostname, ip_buffer);
        return ip_buffer;
    }

    return NULL;
}

static inline int is_host_reachable(const char *ip, uint16_t port) {
    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror(PURPLE "[NETUTILS]" RESET " Socket creation failed");
        return 0;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        perror(PURPLE "[NETUTILS]" RESET " Invalid IP");
        close(sockfd);
        return 0;
    }

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf(PURPLE "[NETUTILS]" RESET " Host %s:%d is unreachable!\n", ip, port);
        close(sockfd);
        return 0;
    }

    printf(PURPLE "[NETUTILS]" RESET " Host %s:%d is reachable!\n", ip, port);
    close(sockfd);
    return 1;
}

// IPv4 version
static inline int hostname_to_all_ips(const char *hostname, char ip_list[][INET_ADDRSTRLEN], int max_ips) {
    struct hostent *he;
    struct in_addr **addr_list;

    if((he = gethostbyname(hostname)) == NULL) {
        perror(PURPLE "[NETUTILS]" RESET " Failed to resolve hostname");
        return 0;
    }

    addr_list = (struct in_addr **)he->h_addr_list;
    int count = 0;

    for(int i = 0; addr_list[i] != NULL && count < max_ips; i++) {
        strncpy(ip_list[count], inet_ntoa(*addr_list[i]), INET_ADDRSTRLEN);
        ip_list[count][INET_ADDRSTRLEN-1] = '\0';
        printf(PURPLE "[NETUTILS]" RESET " %s resolved to %s\n", hostname, ip_list[count]);
        count++;
    }

    return count;
}

// IPv6 Version
static inline int hostname_to_all_ips6(const char *hostname, char ip_list[][INET6_ADDRSTRLEN], int max_ips) {
    struct addrinfo hints, *res, *p;
    int count;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6; // IPv6
    hints.ai_socktype = SOCK_STREAM;
    count = 0;

    if(getaddrinfo(hostname, NULL, &hints, &res) != 0) {
        perror(PURPLE "[NETUTILS]" RESET " Failed to resolve hostname (IPv6)");
        return 0;
    }

    for(p = res; p != NULL && count < max_ips; p = p->ai_next) {
        void *addr = &((struct sockaddr_in6 *)p->ai_addr)->sin6_addr;
        inet_ntop(AF_INET6, addr, ip_list[count], INET6_ADDRSTRLEN);
        printf(PURPLE "[NETUTILS]" RESET " %s resolved to %s\n", hostname, ip_list[count]);
        count++;
    }

    freeaddrinfo(res);
    return count;
}


#endif // NETUTILS_H
