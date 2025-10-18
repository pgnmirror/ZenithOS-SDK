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

#ifndef _ZENITHOS_IPC_H
#define _ZENITHOS_IPC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int zenithos_ipc_create_socket(const char *socket_path) {
    int socket_fd;
    struct sockaddr_un addr;

    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("socket");
        return -1;
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    unlink(socket_path);

    if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        close(socket_fd);
        return -1;
    }

    return socket_fd;
}

// listen connections
int zenithos_ipc_listen(int socket_fd) {
    if (listen(socket_fd, 1) == -1) {
        perror("listen");
        return -1;
    }
    return 0;
}

// accept connection on uniz socket
int zenithos_ipc_accept(int socket_fd) {
    int client_fd = accept(socket_fd, NULL, NULL);
    if (client_fd == -1) {
        perror("accept");
        return -1;
    }
    return client_fd;
}

// send through socket
int zenithos_ipc_send(int socket_fd, const void *data, size_t size) {
    ssize_t bytes_sent = send(socket_fd, data, size, 0);
    if (bytes_sent == -1) {
        perror("send");
        return -1;
    }
    return 0;
}

// receive
int zenithos_ipc_receive(int socket_fd, void *buffer, size_t buffer_size) {
    ssize_t bytes_received = recv(socket_fd, buffer, buffer_size, 0);
    if (bytes_received == -1) {
        perror("recv");
        return -1;
    }
    return bytes_received;
}

//close socket
int zenithos_ipc_close(int socket_fd) {
    if (close(socket_fd) == -1) {
        perror("close");
        return -1;
    }
    return 0;
}

#endif /* _ZENITHOS_IPC_H */
