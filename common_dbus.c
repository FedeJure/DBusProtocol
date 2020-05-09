// Copyright [2020] <Federico Jure>
#include <stdio.h>
#include <string.h>
#include "./common_dbus.h"
#include "./common_socket.h"
#include "./common_utils.h"

#define READ_SIZE_4 4

void dbus_init(dbus_data_t *self, char ***data, char ***body_data) {
  for (size_t i = 0; i < MAX_PARAMS_COUNT; i++) {
    self->params[i].type = 0x00;
    self->params[i].length = 0;
    self->params[i].data_type = 0x00;
  }
  self->body_data = body_data;
  self->params_data = data;
  self->params_count = 0;
  self->body_length = 0;
  self->endianess = 'l';
  self->flag = 0x00;
  self->id = 1;
  self->type = 0;
  self->version = 0;
  self->signature_count = 0;
}

int dbus_read_header(dbus_data_t *self, int client_fd) {
  if (_read_header_general_data(self, client_fd) == DBUS_ERROR) {
    return DBUS_ERROR;
  }
  if (_read_parameters(self, client_fd) == DBUS_ERROR) {
    return DBUS_ERROR;
  }
  return DBUS_SUCCESS;
}

int _read_header_general_data(dbus_data_t *self, int client_fd) {
  char buffer[HEADER_STATIC_SIZE];
  if (socket_read(client_fd, buffer, HEADER_STATIC_SIZE) < 0) {
    return DBUS_ERROR;
  };
  self->endianess = buffer[0];
  self->type = buffer[1];
  self->flag = buffer[2];
  self->version = buffer[3];
  self->body_length = (unsigned int)buffer[READ_SIZE_4];
  self->id = (unsigned int)buffer[READ_SIZE_4 + sizeof(client_fd)];
  self->array_length = ((unsigned int)buffer[READ_SIZE_4
                        + 2 * sizeof(client_fd)]);
  return DBUS_SUCCESS;
}

int _read_parameters(dbus_data_t *self, int client_fd) {
  int index = 0;
  int bytes_readed = 0;
  while (bytes_readed < round_up_eigth(self->array_length)) {
    char buffer[READ_SIZE_4];
    int readed = socket_read(client_fd, buffer, READ_SIZE_4);
    if (readed == SOCKET_ERROR) {
      return DBUS_ERROR;
    }
    bytes_readed += readed;
    self->params[index].type = buffer[0];
    self->params[index].data_type = buffer[2];
    if (self->params[index].type == 0x8) {
      char params_count;
      readed = socket_read(client_fd, &params_count, 1);
      if (readed == SOCKET_ERROR) {
        return DBUS_ERROR;
      }
      bytes_readed += readed;
      self->signature_count = params_count;
      int length_to_read = round_up_eigth(params_count + 6) -
                           5;
      char* params_types = malloc(length_to_read + 1);

      readed = socket_read(client_fd, params_types, length_to_read);
      if (readed == SOCKET_ERROR) {
        return DBUS_ERROR;
      }
      bytes_readed += readed;
      free(params_types);
    } else {
      char readed_length[READ_SIZE_4];
      readed = socket_read(client_fd, readed_length, READ_SIZE_4);
      if (readed == SOCKET_ERROR) {
        return DBUS_ERROR;
      }
      bytes_readed += readed;
      self->params[index].length = (unsigned int)readed_length[0];
      int rounded_length = round_up_eigth(self->params[index].length + 1);
      (*self->params_data)[index] =
          realloc((*self->params_data)[index], rounded_length);
      readed =
          socket_read(client_fd, (*self->params_data)[index], rounded_length);
      if (readed == SOCKET_ERROR) {
        return DBUS_ERROR;
      }
      bytes_readed += readed;
    }
    self->params_count++;
    ++index;
  };
  return DBUS_SUCCESS;
}

int dbus_read_body(dbus_data_t *self, int client_fd) {
  char buffer[READ_SIZE_4];
  int index = 0;
  while (index < self->signature_count) {
    if (socket_read(client_fd, buffer, READ_SIZE_4) == SOCKET_ERROR) {
      return DBUS_ERROR;
    }
    int length = (index == (self->signature_count - 1))
                     ? buffer[0] + 1
                     : round_up_eigth(buffer[0] + 1);
    (*self->body_data)[index] = realloc((*self->body_data)[index], length);
    if (socket_read(client_fd, (*self->body_data)[index], length) ==
        SOCKET_ERROR) {
      return DBUS_ERROR;
    }
    index++;
  }
  return DBUS_SUCCESS;
}

int dbus_get_param_size() { return 8; }

int dbus_get_max_params_count() { return MAX_PARAMS_COUNT; }

int round_up_eigth(int to_round) {
  if (to_round % 8 == 0)
    return to_round;
  return (8 - to_round % 8) + to_round;
}

size_t _dbus_build_stream(char **stream, char ***params,
                          unsigned int params_count, unsigned int id) {
  int method_params_count = _dbus_get_method_params_count((*params)[3]);
  char **signature = malloc(method_params_count * sizeof(char *));
  for (size_t i = 0; i < method_params_count; i++) {
    signature[i] = malloc(1);
  }

  char *method_name = malloc(1);
  _dbus_get_signature_method(params, &method_name, &signature,
                             method_params_count);

  size_t static_size = READ_SIZE_4 + 3 * sizeof(__uint32_t);
  size_t header_size = _dbus_get_header_length_no_padding_on_last(
      params, params_count, method_params_count);
  size_t stream_size = static_size +
                       _dbus_get_body_length_no_padding_on_last(
                           &signature, method_params_count) +
                       round_up_eigth(header_size);
  printf("\nsize static: %ld\nsize header: %ld\n size total: %ld\n",
         static_size, header_size, stream_size);
  int stream_pointer = 0;
  *stream = realloc((*stream), stream_size);
  char *stream_chunk = *stream;
  memset(stream_chunk, 0, stream_size);

  _dbus_build_static_header(&stream_chunk, &stream_pointer, &signature,
                            method_params_count, id);
  _dbus_build_variable_header(&stream_chunk, &stream_pointer, params,
                              params_count, header_size);
  _dbus_build_body(&stream_chunk, &stream_pointer, &signature,
                   method_params_count);

  for (size_t i = 0; i < method_params_count; i++) {
    free(signature[i]);
  }
  free(signature);
  free(method_name);
  return stream_size;
}

void _dbus_build_static_header(char **stream_chunk, int *stream_pointer,
                               char ***signature, int method_params_count,
                               __uint32_t id) {
  (*stream_chunk)[(*stream_pointer)++] = 'l';
  (*stream_chunk)[(*stream_pointer)++] = 0x01;
  (*stream_chunk)[(*stream_pointer)++] = 0x0;
  (*stream_chunk)[(*stream_pointer)++] = 0x01;
  _dbus_save_length(stream_chunk, stream_pointer,
                    betole(_dbus_get_body_length_no_padding_on_last(
                        signature, method_params_count)));
  _dbus_save_length(stream_chunk, stream_pointer, betole(id));
}

void _dbus_save_length(char **stream_chunk, int *stream_pointer,
                       __uint32_t num) {
  char *aux = malloc(sizeof(__uint32_t));
  memset(aux, 0, sizeof(__uint32_t));
  save_decimal_as_bytes(&aux, num);
  memcpy(*stream_chunk + *stream_pointer, aux, sizeof(__uint32_t));
  *stream_pointer += sizeof(__uint32_t);
  free(aux);
}

void _dbus_build_variable_header(char **stream_chunk, int *stream_pointer,
                                 char ***params, int params_count,
                                 int variable_header_length) {
  _dbus_save_length(stream_chunk, stream_pointer,
                    betole(variable_header_length));
  char params_types[READ_SIZE_4] = {0x6, 0x1, 0x2, 0x3};
  char params_data_types[READ_SIZE_4] = {'s', 'o', 's', 's'};
  int order_mapping[READ_SIZE_4] = {1, 0, 2, 3};
  for (size_t i = 0; i < params_count; i++) {
    int index = order_mapping[i];
    (*stream_chunk)[(*stream_pointer)++] = params_types[index];
    (*stream_chunk)[(*stream_pointer)++] = 0x1;
    (*stream_chunk)[(*stream_pointer)++] = params_data_types[index];
    (*stream_pointer)++;
    _dbus_save_length(stream_chunk, stream_pointer,
                      betole(strlen((*params)[index])));
    memcpy((*stream_chunk) + (*stream_pointer), (*params)[index],
           strlen((*params)[index]) + 1);
    (*stream_pointer) += round_up_eigth(strlen((*params)[index]) + 1);
  }
}

void _dbus_build_body(char **stream_chunk, int *stream_pointer,
                      char ***signature, int method_params_count) {
  size_t aux_padding = round_up_eigth(6 + method_params_count);
  int after_initial_body = *stream_pointer + aux_padding;
  (*stream_chunk)[(*stream_pointer)++] = 0x8;
  (*stream_chunk)[(*stream_pointer)++] = 0x1;
  (*stream_chunk)[(*stream_pointer)++] = 'g';
  (*stream_pointer)++;
  (*stream_chunk)[(*stream_pointer)++] = method_params_count;
  for (size_t i = 0; i < method_params_count; i++) {
    (*stream_chunk)[(*stream_pointer)++] = 's';
  }
  *stream_pointer = after_initial_body;

  for (size_t i = 0; i < method_params_count; i++) {
    _dbus_save_length(stream_chunk, stream_pointer,
                      betole(strlen((*signature)[i])));
    memcpy((*stream_chunk) + (*stream_pointer), (*signature)[i],
           strlen((*signature)[i]) + 1);
    (*stream_pointer) += (i == method_params_count - 1)
                             ? strlen((*signature)[i]) + 1
                             : round_up_eigth(strlen((*signature)[i]) + 1);
  }
}

int _dbus_get_body_length_no_padding_on_last(char ***signature, int count) {
  size_t length = 0;
  for (size_t i = 0; i < count - 1; i++) {
    length += sizeof(__uint32_t);
    length += round_up_eigth(strlen((*signature)[i]) + 1);
  }
  length += strlen((*signature)[count - 1]) + 1;
  return length + sizeof(__uint32_t);
}

int _dbus_get_header_length_no_padding_on_last(char ***params, int count,
                                               int signature_count) {
  size_t length = 0;
  for (size_t i = 0; i < count; i++) {
    size_t length_and_data = 5 + sizeof(__uint32_t) + strlen((*params)[i]);
    length += ((i == count - 1) && (signature_count == 0))
                  ? length_and_data
                  : round_up_eigth(length_and_data);
  }
  if (signature_count > 0) {
    length += round_up_eigth(sizeof(__uint32_t) + (2 + signature_count));
  }
  return length;
}

void _dbus_get_signature_method(char ***buffer, char **method_name,
                                char ***signature, int params_count) {
  char *rest = (*buffer)[3];
  char *actual;
  _dbus_read_until_separator(method_name, &actual, &rest, "(");
  char **param;
  for (size_t i = 0; i < params_count - 1; i++) {
    param = &((*signature)[i]);
    _dbus_read_until_separator(param, &actual, &rest, ",");
  }
  param = &((*signature)[params_count - 1]);
  _dbus_read_until_separator(param, &actual, &rest, ")");
}

int _dbus_get_method_params_count(char *method) {
  int comma_count = 0;
  for (size_t i = 0; i < strlen(method); i++) {
    comma_count += method[i] == ',' ? 1 : 0;
  }
  return comma_count + 1;
}

void _dbus_read_until_separator(char **destination, char **pointer, char **rest,
                                char *delim) {
  *pointer = __strtok_r(*rest, delim, rest);
  size_t size = *rest - *pointer;
  *destination = realloc(*destination, size);
  memset(*destination, 0, size);
  memcpy(*destination, *pointer, size);
  *pointer += size;
}
