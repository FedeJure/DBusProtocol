// Copyright [2020] <Federico Jure>
#include <stdio.h>
#include <string.h>
#include "./common_dbus_decoder.h"
#include "./common_socket.h"
#include "./common_utils.h"

#define READ_SIZE_4 4
#define METHOD_PARAM_TYPE 0x8

void dbus_decoder_init(dbus_data_t *self, char ***data, char ***body_data) {
  for (size_t i = 0; i < MAX_PARAMS_COUNT; i++) {
    self->params[i].type = 0x00;
    self->params[i].data_type = 0x00;
  }
  self->body_data = body_data;
  self->params_data = data;
  self->body_length = 0;
  self->endianess = 'l';
  self->flag = 0x00;
  self->id = 1;
  self->type = 0;
  self->version = 0;
  self->signature_count = 0;
}

int dbus_decoder_read_header(dbus_data_t *self, int client_fd) {
  if (_dbus_decoder_read_header_general_data(self, client_fd) == DBUS_ERROR) {
    return DBUS_ERROR;
  }
  if (_dbus_decoder_read_parameters(self, client_fd) == DBUS_ERROR) {
    return DBUS_ERROR;
  }
  return DBUS_SUCCESS;
}

int _dbus_decoder_read_header_general_data(dbus_data_t *self, int client_fd) {
  char buffer[HEADER_STATIC_SIZE];
  if (socket_read(client_fd, buffer, HEADER_STATIC_SIZE) < 0) {
    return DBUS_ERROR;
  };
  self->endianess = buffer[0];
  self->type = buffer[1];
  self->flag = buffer[2];
  self->version = buffer[3];
  self->body_length = (unsigned int)buffer[READ_SIZE_4];
  self->id = (unsigned int)buffer[READ_SIZE_4 * 2];
  self->array_length = get_int_from_char_array(&buffer[READ_SIZE_4 * 3]);
  return DBUS_SUCCESS;
}

int _dbus_decoder_read_parameters(dbus_data_t *self, int client_fd) {
  int index = 0;
  int bytes_readed = 0;
  int internal_return = DBUS_SUCCESS;
  while (bytes_readed < round_up_eigth(self->array_length)) {
    char buffer[READ_SIZE_4];
    int readed = socket_read(client_fd, buffer, READ_SIZE_4);
    if (readed == SOCKET_ERROR) { return DBUS_ERROR; }
    bytes_readed += readed;
    self->params[index].type = buffer[0];
    self->params[index].data_type = buffer[2];
    
    if (self->params[index].type == METHOD_PARAM_TYPE) {
      internal_return =
            _dbus_decoder_read_method_param(self, client_fd, &bytes_readed);
    } else {
      internal_return = _dbus_decoder_read_common_param(self, index, client_fd,
                          &bytes_readed);
    }
    if (internal_return == DBUS_ERROR) { return DBUS_ERROR; }
    ++index;
  };
  return DBUS_SUCCESS;
}

int _dbus_decoder_read_common_param(dbus_data_t *self, int index, int client_fd,
                            int* bytes_readed) {
  int param_length =
          _dbus_decoder_read_length_of_stream(client_fd, bytes_readed);
  if (param_length == DBUS_ERROR) { return DBUS_ERROR; }
  _dbus_decoder_read_param_data_of_stream(self, client_fd, index, bytes_readed,
                                  param_length + 1);

 return DBUS_SUCCESS;
}

int _dbus_decoder_read_method_param(dbus_data_t *self, int client_fd,
                            int* bytes_readed ) {
  char params_count;
  int readed = socket_read(client_fd, &params_count, 1);
  if (readed == SOCKET_ERROR) { return DBUS_ERROR; }
  *bytes_readed += readed;
  self->signature_count = params_count;
  int length_to_read = round_up_eigth(params_count + 6) - 5;
  char* params_types = malloc(length_to_read + 1);
  readed = socket_read(client_fd, params_types, length_to_read);
  if (readed == SOCKET_ERROR) { return DBUS_ERROR; }
  *bytes_readed += readed;
  free(params_types);
  return DBUS_SUCCESS;
}


int _dbus_decoder_read_param_data_of_stream(dbus_data_t *self, int client_fd,
                                    int index, int* bytes_readed, int length) {
  int rounded_length = round_up_eigth(length);
  (*self->params_data)[index] =
    realloc((*self->params_data)[index], rounded_length);
  int readed =
      socket_read(client_fd, (*self->params_data)[index], rounded_length);
  if (readed == SOCKET_ERROR) {
    return DBUS_ERROR;
  }
  *bytes_readed += readed;
  return DBUS_SUCCESS;
}

int dbus_decoder_read_body(dbus_data_t *self, int client_fd) {
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
        SOCKET_ERROR) { return DBUS_ERROR; }
    index++;
  }
  return DBUS_SUCCESS;
}

int _dbus_decoder_read_length_of_stream(int client_fd, int* bytes_readed) {
  char readed_length[READ_SIZE_4];
  int readed = socket_read(client_fd, readed_length, READ_SIZE_4);
  if (readed == SOCKET_ERROR) { return DBUS_ERROR; }
  *bytes_readed += readed;
  return (__int32_t)readed_length[0];
}


int dbus_decoder_get_param_size() { return 8; }

int dbus_decoder_get_max_params_count() { return MAX_PARAMS_COUNT; }
