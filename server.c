// Copyright [2020] <Federico Jure>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <strings.h>
#include "./server.h"
#include "./common_dbus.h"

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
        _server_command_receive(&self);
        printf("user connected\n");
    }

    return SUCCESS;
}

int _server_command_receive(server_t* self) {

    size_t static_header_size = dbus_get_static_size();
    int client_fd;
    dbus_data_t data;
    memset(&data, 0, sizeof(data));

    char* buffer = malloc(static_header_size);
    memset(buffer, 0, static_header_size);

    if (socket_accept(self->socket, &client_fd, self->socket->service) < 0) {
        return -1;
    }
    
    
    socket_read(client_fd, buffer, static_header_size);
    dbus_read_static_header(&data, buffer);
    

    printf("primer byte: %s", buffer);


    //char* new_buffer = realloc(buffer, sizeof(*buffer) + size_step);
    //memset(new_buffer + strlen(buffer) - size_step, 0, size_step);
    //_receive_variable_header(client_fd, buffer);
    //_receive_body(client_fd, buffer);

    fflush(stdout);
    return SUCCESS;
}

int _receive_variable_header(int client_fd, char* buffer ) {
    return 0;
}