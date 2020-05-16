// Copyright [2020] <Federico Jure>
#ifndef SERVER_H_
#define SERVER_H_

#include "./common_socket.h"
#include "./server_dbus_decoder.h"
typedef struct {
    socket_t* socket;
} server_t;

/*=================================PUBLIC=====================================*/

int start_server(char* service);

/*=================================PRIVATE====================================*/
int _server_command_receive(server_t* self, int client_fd);
void _print_log(dbus_data_t* data);
void _server_release(char** params_data,
                    int params_data_count, 
                    char** body_data,
                    int body_data_count);
void _write_variable_log(char** variable_log, dbus_data_t* data);
int _server_decode_body(dbus_data_t* data,
                        const int client_fd,
                        char*** body_data,
                        char*** params_data);

#endif  // SERVER_H_
