// Copyright [2020] <Federico Jure>
#include <stdio.h>
#include <string.h>
#include "./common_dbus_encoder.h"
#include "./common_socket.h"
#include "./common_utils.h"

#define READ_SIZE_4 4
#define METHOD_PARAM_TYPE 0x8

size_t _dbus_encoder_build_stream(char **stream, char ***params,
                          unsigned int params_count, unsigned int id) {
  char* complete_method = (*params)[METHOD_PARAM];
  int method_params_count = _dbus_encoder_get_method_params_count(complete_method, strlen(complete_method));
  char **signature = malloc(method_params_count * sizeof(char *));
  for (size_t i = 0; i < method_params_count; i++) { signature[i] = malloc(1); }

  char *method_name = malloc(1);
  _dbus_encoder_get_signature_method(params, &method_name, &signature,
                method_params_count);

  size_t static_size = READ_SIZE_4 + 3 * sizeof(__uint32_t);
  size_t header_size = _dbus_encoder_get_header_length_no_padding_on_last(
      params, params_count, method_params_count);
  size_t stream_size = static_size +
                       _dbus_encoder_get_body_length_no_padding_on_last(
                           &signature, method_params_count) +
                       round_up_eigth(header_size);
  int stream_pointer = 0;
  *stream = realloc((*stream), stream_size);
  char *stream_chunk = *stream;
  memset(stream_chunk, 0, stream_size);

  _dbus_encoder_build_static_header(&stream_chunk, &stream_pointer, &signature,
                            method_params_count, id);
  _dbus_encoder_build_variable_header(&stream_chunk, &stream_pointer, params,
                              params_count, header_size);
  _dbus_encoder_build_body(&stream_chunk, &stream_pointer, &signature,
                   method_params_count);

  for (size_t i = 0; i < method_params_count; i++) {
    free(signature[i]);
  }
  free(signature);
  free(method_name);
  return stream_size;
}

void _dbus_encoder_build_static_header(char **stream_chunk, int *stream_pointer,
                               char ***signature, int method_params_count,
                               __uint32_t id) {
  (*stream_chunk)[(*stream_pointer)++] = 'l';
  (*stream_chunk)[(*stream_pointer)++] = 0x01;
  (*stream_chunk)[(*stream_pointer)++] = 0x0;
  (*stream_chunk)[(*stream_pointer)++] = 0x01;
  _dbus_encoder_save_length(stream_chunk, stream_pointer,
                    betole(_dbus_encoder_get_body_length_no_padding_on_last(
                        signature, method_params_count)));
  _dbus_encoder_save_length(stream_chunk, stream_pointer, betole(id));
}

void _dbus_encoder_save_length(char **stream_chunk, int *stream_pointer,
                       __uint32_t num) {
  char *aux = malloc(sizeof(__uint32_t));
  memset(aux, 0, sizeof(__uint32_t));
  save_decimal_as_bytes(&aux, num);
  memcpy(*stream_chunk + *stream_pointer, aux, sizeof(__uint32_t));
  *stream_pointer += sizeof(__uint32_t);
  free(aux);
}

void _dbus_encoder_build_variable_header(char **stream_chunk, int *stream_pointer,
                                 char ***params, int params_count,
                                 int variable_header_length) {
  _dbus_encoder_save_length(stream_chunk, stream_pointer,
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
    _dbus_encoder_save_length(stream_chunk, stream_pointer,
                      betole(strlen((*params)[index])));
    memcpy((*stream_chunk) + (*stream_pointer), (*params)[index],
           strlen((*params)[index]) + 1);
    (*stream_pointer) += round_up_eigth(strlen((*params)[index]) + 1);
  }
}

void _dbus_encoder_build_body(char **stream_chunk, int *stream_pointer,
                      char ***signature, int method_params_count) {
  char* aux_stream = *stream_chunk;
  char** aux_signature = *signature;
  _dbus_encoder_build_body_header(stream_chunk, stream_pointer,
                      method_params_count);

  for (size_t i = 0; i < method_params_count; i++) {
    size_t signature_length = strlen(aux_signature[i]);
    _dbus_encoder_save_length(stream_chunk, stream_pointer,
                      betole(signature_length));
    memcpy(aux_stream + (*stream_pointer), aux_signature[i],
           signature_length + 1);
    *stream_pointer += (i == method_params_count - 1)
                             ? signature_length + 1
                             : round_up_eigth(signature_length + 1);
  }
}

int _dbus_encoder_build_body_header(char **stream_chunk, int *stream_pointer,
                      int method_params_count) {
  char* aux_stream = *stream_chunk;
  size_t aux_padding = round_up_eigth(6 + method_params_count);
  int after_initial_body = *stream_pointer + aux_padding;
  aux_stream[(*stream_pointer)++] = 0x8;
  aux_stream[(*stream_pointer)++] = 0x1;
  aux_stream[(*stream_pointer)++] = 'g';
  (*stream_pointer)++;
  aux_stream[(*stream_pointer)++] = method_params_count;
  for (size_t i = 0; i < method_params_count; i++) {
    aux_stream[(*stream_pointer)++] = 's';
  }
  *stream_pointer = after_initial_body;
  return DBUS_SUCCESS;
}

int _dbus_encoder_get_body_length_no_padding_on_last(char ***signature, int count) {
  size_t length = 0;
  if (count == 0) return length;
  for (size_t i = 0; i < count - 1; i++) {
    length += sizeof(__uint32_t);
    length += round_up_eigth(strlen((*signature)[i]) + 1);
  }
  length += strlen((*signature)[count - 1]) + 1;
  return length + sizeof(__uint32_t);
}

int _dbus_encoder_get_header_length_no_padding_on_last(char ***params, int count,
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

void _dbus_encoder_get_signature_method(char ***buffer, char **method_name,
                                char ***signature, int params_count) {
  char *rest = (*buffer)[3];
  char *actual;
  _dbus_encoder_read_until_separator(method_name, &actual, &rest, "(");
  if (params_count == 0) return;
  char **param;
  for (size_t i = 0; i < params_count - 1; i++) {
    param = &((*signature)[i]);
    _dbus_encoder_read_until_separator(param, &actual, &rest, ",");
  }
  param = &((*signature)[params_count - 1]);
  _dbus_encoder_read_until_separator(param, &actual, &rest, ")");
}

int _dbus_encoder_get_method_params_count(char* method, size_t length) {
  int comma_count = 0;
  for (size_t i = 0; i < length; i++) {
    if (method[i] == ')' && method[i-1] == '(') return 0;
    comma_count += method[i] == ',' ? 1 : 0;
  }
  return comma_count + 1;
}

void _dbus_encoder_read_until_separator(char **destination, char **pointer, char **rest,
                                char *delim) {
  *pointer = __strtok_r(*rest, delim, rest);
  size_t size = *rest - *pointer;
  *destination = realloc(*destination, size);
  memset(*destination, 0, size);
  memcpy(*destination, *pointer, size);
  *pointer += size;
}