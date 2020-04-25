// Copyright [2019] <Federico Jure>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>
#include "./client.h"
#include "./client_reader.h"
#include "./common_socket.h"

#define ERROR 1
#define SUCCESS 0

#define BUFFER_SIZE 32

int start_client(char* address, char* service, FILE* entry_file) {
    printf("Starting client...\n");
    fflush(stdout);

    socket_t socket;
    /*if (socket_init(&socket) == -1) {
        return ERROR;
    }
    if (socket_connect(&socket, address, service) == ERROR) {
        return ERROR;
    }*/

    return _process_file(&socket, entry_file);
}

int _process_file(socket_t* socket, FILE* entry_file) {
    reader_t reader;
    init_reader(&reader, entry_file, BUFFER_SIZE);
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    do {
        reader_next_buffer_in_same_line(&reader, buffer);
        _process_buffer(socket, buffer);
        memset(buffer, 0, BUFFER_SIZE);
    } while (reader.reading == 1);

    return SUCCESS;
}

void _process_buffer(socket_t* socket, char* buffer) {
    printf("\n%s",buffer);
}
