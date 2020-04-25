// Copyright [2019] <Federico Jure>
#ifndef CLIENT_H_
#define CLIENT_H_
#include "./common_socket.h"

int start_client(char* address, char* service, FILE* entry_file);
int _process_file(socket_t* socket, FILE* entry_file);
void _process_buffer(socket_t* socket, char* buffer, char* to_send);
void _send_message(socket_t* socket, char* to_send);


#endif  // CLIENT_H_
