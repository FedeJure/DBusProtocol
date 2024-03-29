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
    __uint32_t version;
    __uint32_t body_length;
    __uint32_t id;
    __uint32_t array_length;
    dbus_params_t params[MAX_PARAMS_COUNT];
    char*** params_data;
    char*** body_data;
    int signature_count;
} dbus_data_t;


/*=================================PUBLIC=====================================*/
/*
Recbie una referencia al objeto dbus y el file descriptor del cliente conectado
y guarda en self los datos del header recibidos del cliente.
*/
int dbus_decoder_read_header(dbus_data_t* self, int client_fd);
/*
Recbie una referencia al objeto dbus y el file descriptor del cliente conectado
y guarda en self los datos del body recibidos del cliente.
*/
int dbus_decoder_read_body(dbus_data_t* self, int client_fd);

/*=================================PRIVATE====================================*/

int _dbus_decoder_read_header_general_data(dbus_data_t* self, int client_fd);
int _dbus_decoder_read_parameters(dbus_data_t* self, int client_fd);
void dbus_decoder_init(dbus_data_t* self, char*** data, char*** body_data);
int _dbus_decoder_read_method_param(dbus_data_t *self,
                                    int client_fd,
                                    int* bytes_readed);
int _dbus_decoder_read_common_param(dbus_data_t *self,
                                    int index,
                                    int client_fd,
                                    int* bytes_readed);
int _dbus_decoder_read_param_data_of_stream(dbus_data_t *self,
                                            int client_fd,
                                            int index,
                                            int* bytes_readed,
                                            int length);
int _dbus_decoder_read_length_of_stream(int client_fd, int* bytes_readed);
#endif  // COMMON_DBUS_H_
