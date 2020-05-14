// Copyright [2020] <Federico Jure>
#ifndef COMMON_DBUS_ENCODER_H_
#define COMMON_DBUS_ENCODER_H_

#include <stdio.h>
#define DBUS_ERROR -1
#define DBUS_SUCCESS 0


typedef enum dbus_params {
    DEST_PARAM,
    PATH_PARAM,
    INTERFACE_PARAM,
    METHOD_PARAM
} dbus_params_index;

/*=================================PUBLIC=====================================*/

size_t dbus_encoder_build_stream(char** stream,
                                char*** params,
                                unsigned int params_count,
                                unsigned int id);



/*================================PRIVATE=====================================*/

void _dbus_encoder_calculate_sizes(size_t* static_size,
                                    size_t* header_size,
                                    size_t* stream_size,
                                    char*** params,
                                    int params_count,
                                    int signature_count,
                                    char** signature);
void _dbus_encoder_build_stream(char** stream,
                                size_t stream_size,
                                char** signature,
                                int signature_count,
                                char*** params,
                                int params_count,
                                size_t header_size,
                                __uint32_t id);
int _dbus_encoder_get_body_length_no_padding_on_last(char*** params, int count);
int _dbus_encoder_get_header_length_no_padding_on_last(char*** params,
                                                        int count,
                                                        int signature_count);
void _dbus_encoder_build_static_header(char** stream_chunk,
                                        int* stream_pointer,
                                        char*** signature,
                                        int method_params_count,
                                        __uint32_t id);
void _dbus_encoder_build_variable_header(char** stream_chunk,
                                        int* stream_pointer,
                                        char*** params,
                                        int params_count,
                                        int variable_header_length);
void _dbus_encoder_build_body(char** stream_chunk,
                                int* stream_pointer,
                                char*** signature,
                                int method_params_count);
int _dbus_encoder_build_body_header(char **stream_chunk,
                                    int *stream_pointer,
                                    int method_params_count);
void _dbus_encoder_get_signature_method(char*** buffer,
                                        char** method_name,
                                        char*** signature,
                                        int params_count);
int _dbus_encoder_get_method_params_count(char* method, size_t length);
void _dbus_encoder_read_until_separator(char** destination,
                                        char** pointer,
                                        char** rest,
                                        char* delim);
void _dbus_encoder_save_length(char** stream_chunk,
                                int* stream_pointer,
                                __uint32_t num);

#endif  // COMMON_DBUS_ENCODER_H_
