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

    for (size_t i = 0; i < dbus_get_max_params_count(); i++) { free(params_data[i]); }
    free(params_data);
    return SUCCESS;
}

int _receive_variable_header(int client_fd, char* buffer ) {
    return 0;
}

void print_log(dbus_data_t* data ) {
    char* static_log = 
        "* Id: 0x%04d\n"
        "* Destino: %s\n"
        "* Path: %s\n"
        "* Interfaz: %s\n"
        "* Método: %s%s";
    
    char* variable_log = malloc(1);
    memset(variable_log, 0, 1);
    _write_variable_log(&variable_log, data);

    printf(static_log, data->id, (*data->params_data)[1], (*data->params_data)[0],
        (*data->params_data)[2], (*data->params_data)[3], variable_log);
    fflush(stdout);
    free(variable_log);
}

void _write_variable_log(char** variable_log, dbus_data_t* data) {
    if (data->params_count > 4) {
        char* prefix = "\n* Parámetros:";
        *variable_log = realloc(*variable_log, strlen(prefix) + 1);
        memcpy(*variable_log, prefix, strlen(prefix) + 1);
        for (size_t i = 4; i < data->params_count; i++) {
            size_t actual_size = strlen(*variable_log) + 1;
            size_t new_size = actual_size + data->params[i].length + 1;
            *variable_log = realloc(*variable_log, new_size);
            char* aux_prefix = "\n  * %s";
            char* aux = malloc(data->params[i].length + strlen(aux_prefix) + 1);
            sprintf(aux, aux_prefix, (*data->params_data)[i]);
            memcpy(*variable_log, aux, strlen(aux) + 1);
            free(aux);
        }
                
    }
}