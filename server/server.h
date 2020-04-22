// Copyright [2019] <Federico Jure>
#ifndef _SERVER_E
#define _SERVER_E
#include "./socket.h"

typedef struct {
    socket_t* socket;
} server_t;

int start_server(char* service);
int server_command_receive(server_t* self);
int _server_proccess_command(server_t* self, int client_fd, const char* command);
int _server_proccess_get_command(server_t* self, int client_fd);
int _server_proccess_put_command(server_t* self, int client_fd);
int _server_send_board(server_t* self, int fd);
int _server_proccess_reset_command(server_t* self, int client_fd);
int _server_proccess_verify_command(server_t* self, int client_fd);
#endif