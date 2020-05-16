// Copyright [2020] <Federico Jure>
#ifndef client_dbus_encoder_H_
#define client_dbus_encoder_H_

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
                                const __uint32_t params_count,
                                const __uint32_t id);



/*================================PRIVATE=====================================*/

void _dbus_encoder_calculate_sizes(size_t* static_size,
                                    size_t* header_size,
                                    size_t* stream_size,
                                    char*** params,
                                    const __uint32_t params_count,
                                    const __uint32_t signature_count,
                                    char** signature);
void _dbus_encoder_build_stream(char** stream,
                                size_t stream_size,
                                char** signature,
                                const __uint32_t signature_count,
                                char*** params,
                                const __uint32_t params_count,
                                size_t header_size,
                                __uint32_t id);
int _dbus_encoder_get_body_length_no_padding_on_last(char*** params, const __uint32_t count);
int _dbus_encoder_get_header_length_no_padding_on_last(char*** params,
                                                        const __uint32_t count,
                                                        const __uint32_t signature_count);
void _dbus_encoder_build_static_header(char** stream_pointer,
                                        char*** signature,
                                        const __uint32_t method_params_count,
                                        __uint32_t id);
void _dbus_encoder_build_variable_header(char** stream_pointer,
                                        char*** params,
                                        const __uint32_t params_count,
                                        const __uint32_t variable_header_length);
void _dbus_encoder_build_body(char** stream_pointer,
                                char** signature,
                                const __uint32_t signature_count);
int _dbus_encoder_build_body_header(char **stream_pointer,
                                    const __uint32_t method_params_count);
void _dbus_encoder_get_signature_method(char*** buffer,
                                        char** method_name,
                                        char*** signature,
                                        const __uint32_t params_count);
int _dbus_encoder_get_method_params_count(char* method, size_t length);
void _dbus_encoder_read_until_separator(char** destination,
                                        char** pointer,
                                        char** rest,
                                        char* delim);
void _dbus_encoder_save_length(char** stream_pointer, __uint32_t num);

#endif  // client_dbus_encoder_H_
