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
/*
Recibe los parametros necesarios para armar el mensaje respetando el protocolo
Dbus (https://dbus.freedesktop.org/doc/dbus-specification.html).
Guarda en stream el mensaje armado.
*/
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
/*
Recibe un array con la firma del metodo a enviar, y calcula y retorna
el largo final del body. Agrega padding al calculo del largo de cada
parametro hasta que el offset sea multiplo de 8, exepto al ultimo
parametro, el cual no lleva padding.
*/
int _dbus_encoder_get_body_length_no_padding_on_last(char** signature,
                                                    const __uint32_t count);

/*
Recibe un array con los parametros de la instruccion a enviar, y retorna
el largo final del header. Agrega padding al calculo del largo de cada
parametro hasta que el offset sea multiplo de 8, exepto al ultimo
parametro, el cual no lleva padding.
*/
int _dbus_encoder_get_header_length_no_padding_on_last(char*** params,
                                        const __uint32_t count,
                                        const __uint32_t signature_count);
/*
Recibe los parametros necesarios para armar la parte estatica del header.
Se compone por los siguientes datos y en orden:
1er byte: endianess
2do byte: tipo de mensaje
3er byte: flag (0x0)
4to byte: versión del protocolo
1er entero: longitud en bytes del cuerpo​.
2do entero: un número serie para identificar el mensaje
*/
void _dbus_encoder_build_static_header(char** stream_pointer,
                                        char** signature,
                                        const __uint32_t method_params_count,
                                        __uint32_t id);
/*
Recibe los parametros necesarios para armar la parte variable del header.
Guarda en el stream de bytes el el header armado.
*/
void _dbus_encoder_build_variable_header(char** stream_pointer,
                                    char*** params,
                                    const __uint32_t params_count,
                                    const __uint32_t variable_header_length);
/*
Recibe la firma del metodo a enviar y arma el body del mensaje y lo guarda
en el stream de bytes.
*/
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
void _dbus_encoder_read_method_name(char **destination,
                                        char **pointer,
                                        char **rest,
                                        char *delim);
void _dbus_encoder_read_signature_param(char **destination,
                                        char **pointer,
                                        char **rest,
                                        char *delim);
void _dbus_encoder_save_length(char** stream_pointer, __uint32_t num);

#endif  // client_dbus_encoder_H_
