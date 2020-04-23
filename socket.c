// Copyright [2019] <Federico Jure>
#ifndef _SOCKET_E_
#define _SOCKET_E_

#define _POSIX_C_SOURCE 200112l

#include "./socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#define ALLOWED_CONNECTIONS 1
#define BUFFER_SIZE 30

#define ERROR -1
#define SUCCESS 0

int socket_init(socket_t* self) {
    self->fd = socket(AF_INET, SOCK_STREAM, 0);
    return SUCCESS;
}

int socket_release(socket_t* self) {
    shutdown(self->fd, SHUT_RDWR);
    close(self->fd);
    self->fd = -1;
    return SUCCESS;
}

int socket_connect(socket_t* self, const char* address, char* service) {
    struct sockaddr_in ip4addr = {0};
    struct addrinfo *ai_list, *ptr = {0};
    struct addrinfo hints = {0};
    int res;
    self->service = service;

    res = getaddrinfo(address, service, &hints, &ai_list);

    if (res < 0 ) return ERROR;

    for (ptr = ai_list; (void*)ptr != NULL; ptr = ptr->ai_next) {
        ip4addr.sin_addr.s_addr = *((uint32_t*) & 
            (((struct sockaddr_in*)ptr->ai_addr)->sin_addr));
        ip4addr.sin_family = AF_INET;
        ip4addr.sin_port = htons(atoi(service));
        res = connect(self->fd, (struct sockaddr*)&ip4addr, sizeof(ip4addr));
        if (res == -1) {
            continue;
        }
    }

    return SUCCESS;
}

int socket_listen(socket_t* self, char* service) {
    struct sockaddr_in ip4addr;
    self->service = service;
    ip4addr.sin_family = AF_INET;
    ip4addr.sin_addr.s_addr = INADDR_ANY;
    ip4addr.sin_port = htons( atoi(service) );
    int binded = bind(self->fd, (const struct sockaddr*)&ip4addr,
        sizeof(ip4addr));
    if (binded == -1) {
        return ERROR;
    }

    int listened = listen(self->fd, ALLOWED_CONNECTIONS);
    if (listened < 0) {
        return ERROR;
    }
    return SUCCESS;
}

int socket_accept(socket_t* self, int* client_fd, char* service) {
    struct sockaddr_in ip4addr;
    ip4addr.sin_family = AF_INET;
    ip4addr.sin_addr.s_addr = INADDR_ANY;
    ip4addr.sin_port = htons( atoi(service) );
    size_t socket_size = sizeof(ip4addr);
    int accepted = accept(self->fd, (struct sockaddr*) &ip4addr,
        (socklen_t*)&socket_size);
    if (accepted < -1) {
        return ERROR;
    }
    *client_fd = accepted;
    return SUCCESS;
}

int socket_read(int client_fd, char* buffer, int size) {
    int readed_size = 0;
    while (readed_size < size) {
        int res = recv(client_fd, (void*)&buffer[readed_size],
            size-readed_size, 0);
        if (res < 0 || res == 0) {
            return ERROR;
        }
        readed_size+=res;
    }

    return readed_size;
}

int socket_send(int socket_fd, const char* buffer, int length) {
    int sent = 0;
    while(sent < length) {
        int sended = send(socket_fd, &buffer[sent], (size_t)length-sent,
            MSG_NOSIGNAL);
        if (sended < 0) {
            return ERROR;
        }
        sent += sended;
    }
    return sent;
}

int socket_send_next_length(int fd, int length) {
    int32_t conv = ntohl(length);
    char *data = (char*)&conv;
    return socket_send(fd, data, 4);
}

int socket_read_next_length(int fd) {
    uint32_t network_length;
    if (socket_read(fd, (char*)&network_length, 4) < 0) {
        return ERROR;
    };
    return (int)ntohl(network_length);
}

#endif