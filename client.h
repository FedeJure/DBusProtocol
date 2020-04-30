// Copyright [2020] <Federico Jure>
#ifndef CLIENT_H_
#define CLIENT_H_
#include "./common_socket.h"
#include "./client_reader.h"


int start_client(char* address, char* service, FILE* entry_file);
int _process_file(socket_t* socket, FILE* entry_file);
void _process_buffer(socket_t* socket, char* buffer, char* to_send);
void _send_message(socket_t* socket, char* to_send);
void _process_line(socket_t* socket, reader_t* reader);



#endif  // CLIENT_H_
