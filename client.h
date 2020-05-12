// Copyright [2020] <Federico Jure>
#ifndef CLIENT_H_
#define CLIENT_H_
#include "./common_socket.h"
#include "./client_file_reader.h"

/*=================================PUBLIC=====================================*/

int start_client(char* address, char* service, FILE* entry_file);



/*=================================PRIVATE=====================================*/

int _process_file(socket_t* socket, FILE* entry_file);
void _process_buffer(socket_t* socket, char** buffer, char* to_send);
int _process_line(socket_t* socket, reader_t* reader, int id);
int _extract_params_of_line(char*** dest_params, int* params_count,
                            reader_t* reader);
void _receive_response(socket_t* socket, int id);
void _release_params(char*** params, int count);

#endif  // CLIENT_H_
