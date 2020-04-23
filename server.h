// Copyright [2019] <Federico Jure>
#ifndef SERVER_H_
#define SERVER_H_
#include "./common_socket.h"

typedef struct {
    socket_t* socket;
} server_t;

int start_server(char* service);
int server_command_receive(server_t* self);
#endif  // SERVER_H_
