// Copyright [2019] <Federico Jure>
#ifndef CLIENT_H_
#define CLIENT_H_
#include "./common_socket.h"

int start_client(char* address, char* service, char* entry_file);
int _process_message(socket_t* socket, const char* entry_file);

#endif