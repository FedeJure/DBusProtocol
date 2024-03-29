// Copyright [2020] <Federico Jure>
#include <stdio.h>
#include <string.h>
#include "./client_dbus_encoder.h"
#include "./common_socket.h"
#include "./common_utils.h"

#define METHOD_PARAM_TYPE 0x8
#define PARAM_HEADER_SIZE 4
#define MAX_METHOD_NAME_SIZE 100

/*=================================PUBLIC=====================================*/

size_t dbus_encoder_build_stream(char **stream,
                                char ***params,
                                const __uint32_t params_count,
                                const __uint32_t id) {
  char* complete_method = (*params)[METHOD_PARAM];
  int signature_count = _dbus_encoder_get_method_params_count(
                          complete_method, strlen(complete_method));
  char **signature = malloc(signature_count * sizeof(char *));
  for (size_t i = 0; i < signature_count; i++) { signature[i] = malloc(1); }

  char method_name[MAX_METHOD_NAME_SIZE];
  char* method_name_pointer = method_name;
  _dbus_encoder_get_signature_method(params, &method_name_pointer, &signature,
                signature_count);

  size_t static_size, header_size, stream_size;
  _dbus_encoder_calculate_sizes(&static_size, &header_size, &stream_size,
                  params, params_count, signature_count, signature);

  _dbus_encoder_build_stream(stream, stream_size, signature, signature_count,
                              params, params_count, header_size, id);

  for (size_t i = 0; i < signature_count; i++) {
    free(signature[i]);
  }
  free(signature);
  return stream_size;
}

/*=================================PRIVATE====================================*/

void _dbus_encoder_calculate_sizes(size_t* static_size,
                                  size_t* header_size,
                                  size_t* stream_size,
                                  char*** params,
                                  const __uint32_t params_count,
                                  const __uint32_t signature_count,
                                  char** signature) {
  size_t length_size = sizeof(__uint32_t);
  *static_size = PARAM_HEADER_SIZE + 3 * length_size;
  *header_size = _dbus_encoder_get_header_length_no_padding_on_last(
      params, params_count, signature_count);
  *stream_size = *static_size +
                       _dbus_encoder_get_body_length_no_padding_on_last(
                           signature, signature_count) +
                       round_up_eigth(*header_size);
}

void _dbus_encoder_build_stream(char** stream,
                                const size_t stream_size,
                                char** signature,
                                const __uint32_t signature_count,
                                char*** params,
                                const __uint32_t params_count,
                                const size_t header_size,
                                const __uint32_t id) {
  *stream = realloc(*stream, stream_size);
  char* stream_pointer = *stream;
  memset(*stream, 0, stream_size);

  _dbus_encoder_build_static_header(&stream_pointer, signature,
                            signature_count, id);
  _dbus_encoder_build_variable_header(&stream_pointer, params,
                              params_count, header_size);
  if (signature_count > 0){
    _dbus_encoder_build_body(&stream_pointer, signature,
                   signature_count);
  }
}

void _dbus_encoder_build_static_header(char** stream_pointer,
                                      char** signature,
                                      const __uint32_t signature_count,
                                      __uint32_t id) {
  *((*stream_pointer)++) = 'l';
  *((*stream_pointer)++) = 0x01;
  *((*stream_pointer)++) = 0x0;
  *((*stream_pointer)++) = 0x01;
  _dbus_encoder_save_length(stream_pointer,
      force_little_endian(_dbus_encoder_get_body_length_no_padding_on_last(
                        signature, signature_count)));
  _dbus_encoder_save_length(stream_pointer, force_little_endian(id));
}

void _dbus_encoder_save_length(char **stream_pointer,
                              __uint32_t num) {
  size_t size = sizeof(__uint32_t);
  char *aux = malloc(size);
  memset(aux, 0, size);
  save_decimal_as_bytes(&aux, num);
  memcpy(*stream_pointer, aux, size);
  *stream_pointer += size;
  free(aux);
}

void _dbus_encoder_build_variable_header(char** stream_pointer,
                                char*** params,
                                const __uint32_t params_count,
                                const __uint32_t variable_header_length) {
  _dbus_encoder_save_length(stream_pointer,
                      force_little_endian(variable_header_length));
  char params_types[PARAM_HEADER_SIZE] = {0x6, 0x1, 0x2, 0x3};
  char params_data_types[PARAM_HEADER_SIZE] = {'s', 'o', 's', 's'};
  int order_mapping[PARAM_HEADER_SIZE] = {1, 0, 2, 3};
  for (size_t i = 0; i < params_count; i++) {
    int index = order_mapping[i];
    char* param = (*params)[index];
    *((*stream_pointer)++) = params_types[index];
    *((*stream_pointer)++) = 0x1;
    *((*stream_pointer)++) = params_data_types[index];
    *stream_pointer += 1;
    _dbus_encoder_save_length(stream_pointer,
                      force_little_endian(strlen(param)));
    size_t param_size = strlen(param) + END_OF_STRING;
    memcpy(*stream_pointer, param, param_size);
    *stream_pointer += round_up_eigth(param_size);
  }
}

void _dbus_encoder_build_body(char** stream_pointer,
                              char** signature,
                              const __uint32_t signature_count) {
  _dbus_encoder_build_body_header(stream_pointer, signature_count);
  for (size_t i = 0; i < signature_count; i++) {
    size_t signature_length = strlen((*signature));
    size_t signature_size = signature_length + END_OF_STRING;
    _dbus_encoder_save_length(stream_pointer,
                        force_little_endian(signature_length));
    memcpy(*stream_pointer, *signature, signature_size);
    int is_last_param = i == signature_count - 1;
    *stream_pointer += is_last_param
                        ? signature_size
                        : round_up_eigth(signature_size);
    signature += 1;
  }
}

int _dbus_encoder_build_body_header(char **stream_pointer,
                                    const __uint32_t signature_count) {
  size_t aux_padding = round_up_eigth(6 + signature_count);
  char* after_initial_body = *stream_pointer + aux_padding;
  *((*stream_pointer)++) = 0x8;
  *((*stream_pointer)++) = 0x1;
  *((*stream_pointer)++) = 'g';
  *stream_pointer += 1;
  *((*stream_pointer)++) = signature_count;
  for (size_t i = 0; i < signature_count; i++) {
    *((*stream_pointer)++) = 's';
  }
  *stream_pointer = after_initial_body;
  return DBUS_SUCCESS;
}

int _dbus_encoder_get_body_length_no_padding_on_last(char **signature,
                                                    const __uint32_t count) {
  size_t length = 0;
  if (count == 0) return length;
  for (size_t i = 0; i < count; i++) {
    length += sizeof(__uint32_t);
    char* param = signature[i];
    size_t param_size = strlen(param) + END_OF_STRING;
    int is_last_param = i == count - 1;
    length += is_last_param ?
              param_size :
              round_up_eigth(param_size);
  }
  return length;
}

int _dbus_encoder_get_header_length_no_padding_on_last(char ***params,
                                          const __uint32_t count,
                                          const __uint32_t signature_count) {
  size_t length = 0;
  for (size_t i = 0; i < count; i++) {
    size_t length_size = sizeof(__uint32_t);
    size_t options_size = 4; // 3 byte options + \0
    size_t length_and_data = options_size +
                              length_size +
                              strlen((*params)[i]) + END_OF_STRING;
    int is_last_param = ((i == count - 1) && (signature_count == 0));
    length += is_last_param
                ? length_and_data
                : round_up_eigth(length_and_data);
  }
  if (signature_count > 0) {
    size_t signature_length_size = sizeof(__uint32_t);
    size_t signature_byte_param_count_size = 1;
    length += round_up_eigth(signature_length_size +
                              signature_byte_param_count_size +
                              signature_count +
                              END_OF_STRING);
  }
  return length;
}

void _dbus_encoder_get_signature_method(char ***buffer,
                                        char **method_name,
                                        char ***signature,
                                        const __uint32_t params_count) {
  char *rest = (*buffer)[3];
  char *actual;
  _dbus_encoder_read_method_name(method_name, &actual, &rest, "(");
  if (params_count == 0) return;
  char **param;
  for (size_t i = 0; i < params_count; i++) {
    param = &((*signature)[i]);
    int is_last_param = i == params_count - 1;
    char* separator = is_last_param ? ")" : ",";
    _dbus_encoder_read_signature_param(param, &actual, &rest, separator);
  }
}

int _dbus_encoder_get_method_params_count(char* method, const size_t length) {
  int comma_count = 0;
  for (size_t i = 0; i < length; i++) {
    if (method[i] == ')' && method[i-1] == '(') return 0;
    comma_count += method[i] == ',' ? 1 : 0;
  }
  return comma_count + 1;
}

void _dbus_encoder_read_method_name(char **destination,
                                        char **pointer,
                                        char **rest,
                                        char *delim) {
  *pointer = __strtok_r(*rest, delim, rest);
  size_t size = *rest - *pointer;
  memset(*destination, 0, size);
  memcpy(*destination, *pointer, size);
  *pointer += size;
}

void _dbus_encoder_read_signature_param(char **destination,
                                        char **pointer,
                                        char **rest,
                                        char *delim) {
  *pointer = __strtok_r(*rest, delim, rest);
  size_t size = *rest - *pointer;
  *destination = realloc(*destination, size);
  memset(*destination, 0, size);
  memcpy(*destination, *pointer, size);
  *pointer += size;
}
