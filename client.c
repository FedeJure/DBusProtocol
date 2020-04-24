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

    return _process_message(&socket, entry_file);
}

int _process_message(socket_t* socket, FILE* entry_file) {
    reader_t reader;
    size_t bytes = 32;
    init_reader(&reader, entry_file, bytes);
    char* buffer[bytes];

    do
    {
        reader_next_buffer(&reader, buffer);
        //process buffer
        printf("%s\n", *buffer);
        memset(buffer, 0, bytes);
    } while (reader.reading == 1);

    return SUCCESS;
}
