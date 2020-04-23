// Copyright [2019] <Federico Jure>
#ifndef COMMON_SOCKET_H_
#define COMMON_SOCKET_H_
#include <stdlib.h>

typedef struct socket {
    int fd;
    char* service;
} socket_t;

int socket_init(socket_t* self);
int socket_release(socket_t* self);
int socket_connect(socket_t* self, const char* address, char* service);
int socket_listen(socket_t* self, char* service);
int socket_read(int client_fd, char* buffer, int size);
int socket_accept(socket_t* self, int* client_fd, char* service);
int socket_send(int socket_fd, const char* buffer, int length);
int socket_send_next_length(int fd, int length);
int socket_read_next_length(int fd);

#endif