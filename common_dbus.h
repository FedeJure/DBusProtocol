// Copyright [2020] <Federico Jure>
#ifndef COMMON_DBUS_H_
#define COMMON_DBUS_H_

#include <stdio.h>

#define MAX_PARAMS_COUNT 5
#define DBUS_ERROR -1
#define DBUS_SUCCESS 0
#define HEADER_STATIC_SIZE 4 + 3 * sizeof(int)

typedef struct params {
    char type;
    char data_type;
    int length;
} dbus_params_t;

typedef struct dbus {
    char endianess;
    int type;
    int flag;
    unsigned int version;
    unsigned int body_length;
    unsigned int id;
    unsigned int array_length;
    dbus_params_t params[MAX_PARAMS_COUNT];
    char*** params_data;
    char*** body_data;
    int signature_count;
} dbus_data_t;

typedef enum dbus_params {
    DEST_PARAM,
    PATH_PARAM,
    INTERFACE_PARAM,
    METHOD_PARAM
} dbus_params_index;


int dbus_read_header(dbus_data_t* self, int client_fd);
int _read_header_general_data(dbus_data_t* self, int client_fd);
int _read_parameters(dbus_data_t* self, int client_fd);
int dbus_read_body(dbus_data_t* self, int client_fd);
int dbus_get_static_size();
int dbus_get_param_size();
void dbus_init(dbus_data_t* self, char*** data, char*** body_data);
int dbus_get_max_params_count();
int round_up_eigth(int to_round);
size_t _dbus_build_stream(char** stream, char*** params,
                        unsigned int params_count, unsigned int id);
int _dbus_get_body_length_no_padding_on_last(char*** params, int count);
int _dbus_get_header_length_no_padding_on_last(char*** params,
                        int count, int signature_count);
void _dbus_build_static_header(char** stream_chunk, int* stream_pointer,
                                char*** signature, int method_params_count,
                                __uint32_t id);
void _dbus_build_variable_header(char** stream_chunk, int* stream_pointer,
                                char*** params, int params_count,
                                int variable_header_length);
void _dbus_build_body(char** stream_chunk, int* stream_pointer,
                        char*** signature, int method_params_count);
int _dbus_build_body_header(char **stream_chunk, int *stream_pointer,
                      int method_params_count);
void _dbus_get_signature_method(char*** buffer, char** method_name,
                        char*** signature, int params_count);
int _dbus_get_method_params_count(char* method, size_t length);
void _dbus_read_until_separator(char** destination, char** pointer,
                                char** rest, char* delim);
void _dbus_save_length(char** stream_chunk, int* stream_pointer,
                        __uint32_t num);
int _dbus_read_method_param(dbus_data_t *self, int client_fd,
                        int* bytes_readed );
int _dbus_read_common_param(dbus_data_t *self, int index,
                        int client_fd, int* bytes_readed);
int _dbus_read_length_of_stream(int client_fd, int* bytes_readed);
int _dbus_read_param_data_of_stream(dbus_data_t *self, int client_fd,
                                    int index, int* bytes_readed, int length);
#endif  // COMMON_DBUS_H_
