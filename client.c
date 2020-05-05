// Copyright [2020] <Federico Jure>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>
#include "./client.h"
#include "./client_reader.h"
#include "./common_socket.h"
#include "./common_dbus.h"

#define ERROR 1
#define SUCCESS 0

#define BUFFER_SIZE 32
#define PARAMS_COUNT 4

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
    char** params = malloc(sizeof(char*) * PARAMS_COUNT);
    bool early_return = false;
    int params_count = PARAMS_COUNT;
    for (size_t i = 0; i < params_count; i++){
        params[i] = malloc(1);
        reader_next_buffer_until_space(reader, &params[i]);
        printf("%ld\n", strlen(params[i]));
        printf("%s\n", params[i]);
        if (reader->reading == false || strlen(params[i]) <= 1) {
            reader->reading = false;
            early_return = true;
            params_count = i + 1;
            break;
        }
    }
    if (early_return == false) { _dbus_build_stream(&params, 1); }
    // _send_message(socket, line);
    for (size_t i = 0; i < params_count; i++) { free(params[i]); }
    free(params);
    
}

void _process_buffer(socket_t* socket, char** buffer, char* to_send) {
    memcpy(to_send, *buffer, strlen(*buffer) + 1);
}

void _send_message(socket_t* socket, char* to_send) {
    socket_send(socket->fd, to_send, strlen(to_send));
}