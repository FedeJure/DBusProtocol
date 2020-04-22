// Copyright [2019] <Federico Jure>
#ifndef _CLIENT_E
#define _CLIENT_E
#include "./socket.h"

int start_client(char* address, char* service);
int _proccess_command(socket_t* socket, const char* command);
int _client_proccess_get(socket_t* socket, const char * buffer);
int _client_proccess_put(socket_t* socket, const char* buffer);
int _client_proccess_verify(socket_t* socket, const char* buffer);
int _client_proccess_reset(socket_t* socket, const char* buffer);
int _client_proccess_exit(socket_t* socket, const char* buffer);

#endif