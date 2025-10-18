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

#ifndef QRTR_H
#define QRTR_H

#if defined(__GNUC__) || defined(__clang__)
#  warning "qrtr.h is soon DEPRECATED! Avoid using it in new code."
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

// QRTR message types
#define QRTR_MSG_TYPE_REQUEST 0x01
#define QRTR_MSG_TYPE_RESPONSE 0x02
#define QRTR_MSG_TYPE_NOTIFICATION 0x03

// Max buffer size for QRTR messages
#define QRTR_MAX_MSG_SIZE 1024

// QRTR structure for a message
struct qrtr_message {
    uint32_t msg_type;         // Message type (request, response, etc.)
    uint32_t client_id;        // Client ID
    uint32_t service_id;       // Service ID to interact with
    uint32_t msg_length;       // Length of the message data
    char data[QRTR_MAX_MSG_SIZE]; // Message data
};

// QRTR socket structure
struct qrtr_socket {
    int sockfd;                // Socket file descriptor
    struct sockaddr_in server_addr; // Server address for QRTR
};

// Function to create a QRTR socket
int qrtr_create_socket(const char *server_ip, uint16_t port) {
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid server IP");
        close(sockfd);
        return -1;
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

// Function to send a message over QRTR
int qrtr_send_message(int sockfd, struct qrtr_message *msg) {
    ssize_t sent_bytes = send(sockfd, msg, sizeof(*msg), 0);
    if (sent_bytes < 0) {
        perror("Error sending message");
        return -1;
    }

    printf("Message sent: %d bytes\n", (int)sent_bytes);
    return 0;
}

// Function to receive a message over QRTR
int qrtr_receive_message(int sockfd, struct qrtr_message *msg) {
    ssize_t recv_bytes = recv(sockfd, msg, sizeof(*msg), 0);
    if (recv_bytes < 0) {
        perror("Error receiving message");
        return -1;
    }

    printf("Message received: %d bytes\n", (int)recv_bytes);
    return 0;
}

// Function to close QRTR socket
void qrtr_close_socket(int sockfd) {
    close(sockfd);
    printf("Socket closed\n");
}

#endif // QRTR_H
