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
    if (socket_init(&socket) == -1) {
        return ERROR;
    }
    if (socket_connect(&socket, address, service) == ERROR) {
        return ERROR;
    }

    return _process_file(&socket, entry_file);
}

int _process_file(socket_t* socket, FILE* entry_file) {
    reader_t reader;
    init_reader(&reader, entry_file, BUFFER_SIZE);

    while (reader.reading == true)
    {
        _process_line(socket, &reader);        
    }
    


    return SUCCESS;
}

void _process_line(socket_t* socket, reader_t* reader) {
    char* line = malloc(1);
    bool line_break = false;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    memset(line, 0, 1);
    while (line_break == 0) {
        line = realloc(line, strlen(line) + BUFFER_SIZE - 1);
        memset(line + strlen(line), 0, BUFFER_SIZE);

        reader_next_buffer_in_same_line(reader, buffer, &line_break);
        _process_buffer(socket, buffer, line);
        memset(buffer, 0, BUFFER_SIZE);
    }
    printf("%s\n", line);
    _send_message(socket, line);
    free(line);
}

void _process_buffer(socket_t* socket, char* buffer, char* to_send) {
    memcpy(to_send + strlen(to_send), buffer, strlen(buffer) + 1);
}

void _send_message(socket_t* socket, char* to_send) {
    socket_send(socket->fd, to_send, strlen(to_send));
}