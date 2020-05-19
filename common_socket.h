// Copyright [2020] <Federico Jure>
#ifndef COMMON_SOCKET_H_
#define COMMON_SOCKET_H_
#include <stdlib.h>

#define SOCKET_ERROR -1
#define SOCKET_SUCCESS 0

typedef struct socket {
    int fd;
    char* service;
} socket_t;

int socket_init(socket_t* self);
int socket_release(socket_t* self);
/*
Recibe la referencia al socket, address y service al cual conectarse.
Intenta establecer una conexion y en caso negativo retorna codigo de
error.
*/
int socket_connect(socket_t* self, const char* address, char* service);
/*
Recibe la referencia al socket, y el servicio en el cual iniciarlo
en modo de escucha.
*/
int socket_listen(socket_t* self, char* service);
/*
Lee la cantidad de bytes definido por size, del file descriptor
y guarda lo leído en buffer.
*/
int socket_read(int fd, char* buffer, int size);
/*
Envia la cantidad de bytes definido por length
*/
int socket_send(int socket_fd, const char* buffer, int length);
int socket_accept(socket_t* self, int* fd, char* service);
int socket_send_next_length(int fd, int length);
int socket_read_next_length(int fd);

#endif  // COMMON_SOCKET_H_
