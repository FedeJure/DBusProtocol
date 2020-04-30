// Copyright [2020] <Federico Jure>
#ifndef SERVER_H_
#define SERVER_H_
#include "./common_socket.h"

typedef struct {
    socket_t* socket;
} server_t;

int start_server(char* service);
int _server_command_receive(server_t* self);
int _receive_variable_header(int client_fd, char* buffer);
void _receive_body(int client_fd, char* buffer);

#endif  // SERVER_H_
