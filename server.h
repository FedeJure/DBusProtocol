// Copyright [2020] <Federico Jure>
#ifndef SERVER_H_
#define SERVER_H_
#include "./common_socket.h"
#include "./common_dbus.h"

typedef struct {
    socket_t* socket;
} server_t;

int start_server(char* service);
int _server_command_receive(server_t* self);
int _receive_variable_header(int client_fd, char* buffer);
void _receive_body(int client_fd, char* buffer);
void print_log(dbus_data_t* data);
void _write_variable_log(char** variable_log, dbus_data_t* data);

#endif  // SERVER_H_
