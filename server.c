// Copyright [2019] <Federico Jure>
#include "./server.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <strings.h>

#define ERROR 1
#define SUCCESS 0
#define BUFFER_SIZE 32

int start_server(char* service) {
    printf("Starting server...\n");
    fflush(stdout);
    socket_t socket;
    server_t self;
    self.socket = &socket;

    if (socket_init(&socket) == -1) {
        return ERROR;
    }
    if (socket_listen(&socket, service) == -1) {
        return ERROR;
    }
    while (self.socket->fd != -1) {
        server_command_receive(&self);
        printf("user connected\n");
    }

    return SUCCESS;
}

int server_command_receive(server_t* self) {
    int client_fd;
    char* buffer = malloc(1);
    memset(buffer, ' ', 1);
    if (socket_accept(self->socket, &client_fd, self->socket->service) < 0) {
        return -1;
    }

    while ( socket_read(client_fd, buffer, 32) > 0) {

    }
    printf("leido: %s\n", buffer);
    fflush(stdout);
    return SUCCESS;
}
