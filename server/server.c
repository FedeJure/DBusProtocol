// Copyright [2019] <Federico Jure>
#include "./server.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <strings.h>

#define PUT_LENGTH 12
#define GET "G"
#define PUT "P"
#define VERIFY "V"
#define RESET "R"
#define INVALID_CELL "Celda no valida\n"
#define ERROR 1
#define SUCCESS 0
int start_server(char* service) {
    socket_t socket;
    server_t self;
    self.socket = &socket;
    if (socket_init(&socket) == -1) {
        return ERROR;
    }
    if (socket_listen(&socket, service) == -1) {
        return ERROR;
    }
    while (self.socket->fd != -1) {
        server_command_receive(&self);
    }

    return SUCCESS;
}

int server_command_receive(server_t* self) {
    int client_fd;
    char buffer[1];
    bzero(buffer, 1);
    if (socket_accept(self->socket, &client_fd, self->socket->service) == -1) {
        return -1;
    }

    while (client_fd != -1) {
        if (socket_read(client_fd, buffer, 1) < 0) {
            break;
        }
        if (_server_proccess_command(self, client_fd, buffer) == ERROR) {
            break;
        }
    }

    return SUCCESS;
}

int _server_proccess_command(server_t* self, int client_fd, const char* command) {
    if (strcmp(command, GET) == 0) {
        return _server_proccess_get_command(self, client_fd);
    }
    else if (strcmp(command, PUT) == 0) {
        return _server_proccess_put_command(self, client_fd);
    }
    else if (strcmp(command, RESET) == 0) {
        return _server_proccess_reset_command(self, client_fd);
    }
    else if (strcmp(command, VERIFY) == 0) {
        return _server_proccess_verify_command(self, client_fd);
    }
    return SUCCESS;
}

int _server_build_board_to_send(sudoku_t* sudoku, int size, int*** values ) {
    sudoku_get_board(sudoku, values);
    return SUCCESS;
}

int _server_proccess_get_command(server_t* self, int client_fd) {
    return _server_send_board(self, client_fd);
}

int _server_proccess_put_command(server_t* self, int client_fd) {
    char input[3];
    if (socket_read(client_fd, input, 3) < 0) {
        return -1;
    };
    int row = input[0];
    int column = input[1];
    int value = input[2];
    int res = sudoku_put_in_position(self->sudoku, value, row, column);
    if (res != SUCCESS) {
        char* response = INVALID_CELL;
        int length = strlen(response);

        if ( socket_send_next_length(client_fd, length) < 0) {
            return ERROR;
        }
        if ( socket_send(client_fd, response, length) < 0) {
            return ERROR;
        }
    }
    return _server_send_board(self, client_fd);
}

int _server_send_board(server_t* self, int fd) {
    char* board = malloc(sizeof(char)*1000);
    bzero(board, sizeof(char)*1000);
    sudoku_draw(self->sudoku, board);
    int length = strlen(board);

    if ( socket_send_next_length(fd, length) < 0) {
        free(board);
        return ERROR;
    }
    if ( socket_send(fd, board, length) < 0) {
        free(board);
        return ERROR;
    }

    free(board);
    return SUCCESS;
}

int _server_proccess_reset_command(server_t* self, int client_fd) {
    sudoku_clean(self->sudoku);
    return _server_send_board(self, client_fd);
}

int _server_proccess_verify_command(server_t* self, int client_fd) {
    char* response;
    int length;
    int verified = sudoku_verify(self->sudoku);
    if (verified != SUCCESS) {
        response = "ERROR";
    }
    else {
        response = "OK";
    }
    length = strlen(response);
    if (socket_send_next_length(client_fd, length) < 0 ) {
        return ERROR;
    }
    return socket_send(client_fd, response, length);
}