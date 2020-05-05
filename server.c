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
    char** params_data = malloc(MAX_PARAMS_COUNT * sizeof(char*));
    char** body_data = malloc(1 * sizeof(char*));
    for (size_t i = 0; i < MAX_PARAMS_COUNT; i++) { params_data[i] = malloc(1); }
    dbus_init(&data, &params_data, &body_data);
    if (socket_accept(self->socket, &client_fd, self->socket->service) < 0) { return -1; }
    dbus_read_header(&data, client_fd);

    if (data.params_count > MAX_PARAMS_COUNT - 1) {
        body_data = realloc(body_data, data.params[MAX_PARAMS_COUNT].length * sizeof(char*));
        for (size_t i = 0; i < data.params[MAX_PARAMS_COUNT].length; i++){ body_data[i] = malloc(1); }
        dbus_read_body(&data, client_fd);
    }
    
    _print_log(&data);

    char* response = "OK";
    socket_send(client_fd, response, strlen(response) + 1);
    for (size_t i = 0; i < dbus_get_max_params_count(); i++) { free(params_data[i]); }
    if (data.params_count > 4) { for (size_t i = 0; i < data.params[MAX_PARAMS_COUNT].length; i++){ free(body_data[i]); } }
    free(params_data);
    free(body_data);
    return SUCCESS;
}

void _print_log(dbus_data_t* data ) {
    char* static_log = 
        "* Id: 0x%04d\n"
        "* Destino: %s\n"
        "* Path: %s\n"
        "* Interfaz: %s\n"
        "* Método: %s%s\n";
    
    char* variable_log = malloc(1);
    memset(variable_log, 0, 1);
    _write_variable_log(&variable_log, data);

    printf(static_log, data->id, (*data->params_data)[1], (*data->params_data)[0],
        (*data->params_data)[2], (*data->params_data)[3], variable_log);
    fflush(stdout);
    free(variable_log);
}

void _write_variable_log(char** variable_log, dbus_data_t* data) {
    if (data->params_count > MAX_PARAMS_COUNT - 1) {
        char* prefix = "\n* Parámetros:";
        *variable_log = realloc(*variable_log, strlen(prefix) + 1);
        memcpy(*variable_log, prefix, strlen(prefix) + 1);
        for (size_t i = 0; i < data->params[MAX_PARAMS_COUNT-1].length; i++) {
            size_t actual_size = strlen(*variable_log) + 1;
            size_t new_size = actual_size + data->params[i].length + 1;
            *variable_log = realloc(*variable_log, new_size);
            char* aux_prefix = "\n  * %s";
            char* aux = malloc(data->params[i].length + strlen(aux_prefix) + 1);
            sprintf(aux, aux_prefix, (*data->body_data)[i]);
            memcpy(*variable_log, aux, strlen(aux) + 1);
            free(aux);
        }
                
    }
}