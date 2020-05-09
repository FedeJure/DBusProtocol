// Copyright [2020] <Federico Jure>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "./server.h"
#include "./common_dbus.h"

#define SERVER_ERROR 1
#define SERVER_SUCCESS 0
#define BUFFER_SIZE 32

int start_server(char *service) {
  fflush(stdout);
  socket_t socket;
  server_t self;
  self.socket = &socket;

  if (socket_init(&socket) == -1) {
    return SERVER_ERROR;
  }
  if (socket_listen(&socket, service) == -1) {
    return SERVER_ERROR;
  }
  while (self.socket->fd != -1) {
    int client_fd;
    socket_accept(self.socket, &client_fd, self.socket->service);

    while (client_fd > 0) {
      if (_server_command_receive(&self, client_fd) == SERVER_ERROR) {
        client_fd = 0;
      }
    }
  }

  return SERVER_SUCCESS;
}

int _server_command_receive(server_t *self, int client_fd) {
  dbus_data_t data;
  char **params_data = malloc(MAX_PARAMS_COUNT * sizeof(char *));
  char **body_data = malloc(1 * sizeof(char *));
  for (size_t i = 0; i < MAX_PARAMS_COUNT; i++) {
    params_data[i] = malloc(1);
  }
  dbus_init(&data, &params_data, &body_data);
  if (dbus_read_header(&data, client_fd) == DBUS_ERROR) {
    return SERVER_ERROR;
  }

  if (data.params_count > MAX_PARAMS_COUNT - 1) {
    body_data = realloc(body_data, data.signature_count * sizeof(char *));
    for (size_t i = 0; i < data.signature_count; i++) {
      body_data[i] = malloc(1);
    }
    if (dbus_read_body(&data, client_fd) == DBUS_ERROR) {
      return SERVER_ERROR;
    }
  }

  _print_log(&data);

  char *response = "OK";
  if (socket_send(client_fd, response, strlen(response) + 1) == SOCKET_ERROR) {
    return SERVER_ERROR;
  }
  for (size_t i = 0; i < dbus_get_max_params_count(); i++) {
    free(params_data[i]);
  }
  if (data.params_count > 4) {
    for (size_t i = 0; i < data.params[MAX_PARAMS_COUNT - 1].length; i++) {
      free(body_data[i]);
    }
  }
  free(params_data);
  free(body_data);
  return SERVER_SUCCESS;
}

void _print_log(dbus_data_t *data) {
  char *static_log = "* Id: 0x%08d\n"
                     "* Destino: %s\n"
                     "* Path: %s\n"
                     "* Interfaz: %s\n"
                     "* Método: %s%s\n";

  char *variable_log = malloc(1);
  memset(variable_log, 0, 1);
  _write_variable_log(&variable_log, data);

  printf(static_log, data->id, (*data->params_data)[1], (*data->params_data)[0],
         (*data->params_data)[2], (*data->params_data)[3], variable_log);
  fflush(stdout);
  free(variable_log);
}

void _write_variable_log(char **variable_log, dbus_data_t *data) {
  if (data->signature_count > 0) {
    char *prefix = "\n* Parámetros:";
    *variable_log = realloc(*variable_log, strlen(prefix) + 1);
    memcpy(*variable_log, prefix, strlen(prefix) + 1);
    size_t actual_size = strlen(*variable_log);
    for (size_t i = 0; i < data->signature_count; i++) {
      char *aux_prefix = "\n  * %s";
      size_t data_size = strlen((*data->body_data)[i]);
      size_t aux_prefix_size = strlen(aux_prefix);
      char *aux = malloc(data_size + aux_prefix_size + 1);
      memset(aux, 0, data_size + aux_prefix_size + 1);
      snprintf(aux, data_size + aux_prefix_size + 1,
                aux_prefix, (*data->body_data)[i]);
      *variable_log =
          realloc(*variable_log, strlen(*variable_log) + strlen(aux) + 1);
      memcpy(*variable_log + actual_size - 1, aux, strlen(aux));
      actual_size += strlen(aux);
      free(aux);
    }
  }
}
