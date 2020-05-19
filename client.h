// Copyright [2020] <Federico Jure>
#ifndef CLIENT_H_
#define CLIENT_H_
#include "./common_socket.h"
#include "./client_file_reader.h"

/*=================================PUBLIC=====================================*/

/*
Recibe los argumentos ingresados por el usuario e inicia las tareas del cliente
*/
int start_client(char* address, char* service, FILE* entry_file);



/*=================================PRIVATE====================================*/
/*
Procesa el archivo completo provisto por el usuario y envia una por una las
instrucciones al servidor.
*/
int _process_file(socket_t* socket, FILE* entry_file);
/*
Procesa una unica instruccion del archivo de instrucciones provisto por el
usuario, arma el mensaje, y lo envia al servidor a travez de la conexion
de socket.
*/
int _process_line(socket_t* socket, reader_t* reader, int id);
/*
Recibe un array de caracteres de destino, y extrae los parametros de una sola 
instruccion, es decir una linea, del archivo de instrucciones provisto por el
usuario.
*/
int _extract_params_of_line(char*** dest_params,
                            int* params_count,
                            reader_t* reader);
void _process_buffer(socket_t* socket, char** buffer, char* to_send);
void _receive_response(socket_t* socket, int id);
void _release_params(char*** params, int count);

#endif  // CLIENT_H_
