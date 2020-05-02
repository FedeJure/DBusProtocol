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
    int client_fd;
    dbus_data_t data;
    char** params_data = malloc(dbus_get_max_params_count() * sizeof(char*));
    for (size_t i = 0; i < dbus_get_max_params_count(); i++) { params_data[i] = malloc(1); }

    dbus_init(&data, &params_data);

    if (socket_accept(self->socket, &client_fd, self->socket->service) < 0) { return -1; }
    
    dbus_read_buffer(&data, client_fd);
    print_log(&data);
    fflush(stdout);

    for (size_t i = 0; i < dbus_get_max_params_count(); i++) { free(params_data[i]); }
    free(params_data);
    return SUCCESS;
}

int _receive_variable_header(int client_fd, char* buffer ) {
    return 0;
}

void print_log(dbus_data_t* data ) {
    char* aux = 
        "* Id: %d\n"
        "* Destino: %s\n"
        "* Path: %s\n"
        "* Interfaz: %s\n"
        "* Método: %s\n";

    printf(aux, data->id, (*data->params_data)[1], (*data->params_data)[0],
        (*data->params_data)[2], (*data->params_data)[3]);
}