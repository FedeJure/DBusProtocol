// Copyright [2020] <Federico Jure>
#ifndef CLIENT_H_
#define CLIENT_H_
#include "./common_socket.h"
#include "./client_reader.h"


int start_client(char* address, char* service, FILE* entry_file);
int _process_file(socket_t* socket, FILE* entry_file);
void _process_buffer(socket_t* socket, char** buffer, char* to_send);
int _process_line(socket_t* socket, reader_t* reader, int id);
void _receive_response(socket_t* socket, int id);
void _release_params(char*** params, int count);

#endif  // CLIENT_H_
