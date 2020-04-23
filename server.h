// Copyright [2019] <Federico Jure>
#ifndef _SERVER_E
#define _SERVER_E
#include "./common_socket.h"

typedef struct {
    socket_t* socket;
} server_t;

int start_server(char* service);
int server_command_receive(server_t* self);
#endif