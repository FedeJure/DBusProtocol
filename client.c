// Copyright [2019] <Federico Jure>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>
#include "./client.h"
#include "./common_socket.h"

#define ERROR 1
#define SUCCESS 0

int start_client(char* address, char* service, char* entry_file) {
    printf("Starting client...\n");
    fflush(stdout);
    socket_t socket;
    if (socket_init(&socket) == -1) {
        return ERROR;
    }

    if (socket_connect(&socket, address, service) == ERROR) {
        return ERROR;
    }

    return _process_message(&socket, entry_file);
}

int _process_message(socket_t* socket, const char* entry_file) {
    printf("Procesando comando");
    return SUCCESS;
}
