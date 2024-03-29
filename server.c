// Copyright [2020] <Federico Jure>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "./server.h"
#include "./server_dbus_decoder.h"
#include "./common_utils.h"

#define BUFFER_SIZE 32

/*=================================PUBLIC=====================================*/

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
  int client_fd;
  socket_accept(self.socket, &client_fd, self.socket->service);
  bool keep_receive_commands = true;
  while (keep_receive_commands == true) {
    if (_server_command_receive(&self, client_fd) == SERVER_ERROR) {
      keep_receive_commands = false;
    }
  }

  socket_release(&client_fd);
  socket_release(&socket.fd);

  return SERVER_SUCCESS;
}




/*=================================PRIVATE====================================*/

int _server_command_receive(server_t *self, int client_fd) {
  dbus_data_t data;
  char **params_data = malloc(MAX_PARAMS_COUNT * sizeof(char *));
  char **body_data = malloc(1 * sizeof(char *));
  for (size_t i = 0; i < MAX_PARAMS_COUNT; i++) { params_data[i] = malloc(1); }
  dbus_decoder_init(&data, &params_data, &body_data);
  if (dbus_decoder_read_header(&data, client_fd) == DBUS_ERROR) {
    _server_release(params_data, MAX_PARAMS_COUNT,
                  body_data, data.signature_count);
    return SERVER_ERROR;
  }
  if (data.signature_count > 0) {
    if (_server_decode_body(&data, client_fd, &body_data, &params_data)
        == SERVER_ERROR) { return SERVER_ERROR; }
  }
  _print_log(&data);

  char *response = "OK";
  if (socket_send(client_fd, response, strlen(response) + END_OF_STRING)
                                            == SOCKET_ERROR) {
    _server_release(params_data, MAX_PARAMS_COUNT, body_data,
                      data.signature_count);
    return SERVER_ERROR;
  }
  _server_release(params_data, MAX_PARAMS_COUNT, body_data,
                  data.signature_count);
  return SERVER_SUCCESS;
}

int _server_decode_body(dbus_data_t* data,
                        const int client_fd,
                        char*** body_data,
                        char*** params_data) {
  *body_data = realloc(*body_data, data->signature_count * sizeof(char *));
  for (size_t i = 0; i < data->signature_count; i++) {
    (*body_data)[i] = malloc(1);
  }
  if (dbus_decoder_read_body(data, client_fd) == DBUS_ERROR) {
    _server_release(*params_data, MAX_PARAMS_COUNT, *body_data,
                    data->signature_count);
    return SERVER_ERROR;
  }
  return SERVER_SUCCESS;
}

void _server_release(char** params_data,
                      int params_data_count, 
                      char** body_data,
                      int body_data_count) {
  for (size_t i = 0; i < params_data_count; i++) { free(params_data[i]); }
  for (size_t i = 0; i < body_data_count; i++) { free(body_data[i]); }
  free(params_data);
  free(body_data);
}

void _print_log(dbus_data_t *data) {
  char *static_log = "* Id: 0x%08x\n"
                     "* Destino: %s\n"
                     "* Ruta: %s\n"
                     "* Interfaz: %s\n"
                     "* Metodo: %s%s\n\n";

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
    char *prefix = "\n* Parametros:";
    size_t prefix_size = strlen(prefix) + END_OF_STRING;
    *variable_log = realloc(*variable_log, prefix_size);
    memcpy(*variable_log, prefix, prefix_size);
    size_t actual_size = strlen(*variable_log) + END_OF_STRING;
    for (size_t i = 0; i < data->signature_count; i++) {
      char *aux_prefix = "\n    * %s";
      size_t data_size = strlen((*data->body_data)[i]);
      size_t aux_prefix_size = strlen(aux_prefix) + END_OF_STRING;
      char *aux = malloc(data_size + aux_prefix_size);
      memset(aux, 0, data_size + aux_prefix_size);
      snprintf(aux, data_size + aux_prefix_size,
                aux_prefix, (*data->body_data)[i]);
      size_t aux_size = strlen(aux) + END_OF_STRING;
      *variable_log = realloc(*variable_log, strlen(*variable_log) + aux_size);
      memcpy(*variable_log + actual_size - 1, aux, aux_size);
      actual_size += aux_size - END_OF_STRING;
      free(aux);
    }
  }
}
