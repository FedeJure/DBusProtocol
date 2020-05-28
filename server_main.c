// Copyright [2020] <Federico Jure>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./server_main.h"
#include "./server.h"
#include "./signal.h"

#define MAX_ARGUMENTS 1
#define MIN_ARGUMENTS 1

#define ERROR 1
#define SUCCESS 0

int main(int argc, char* argv[]) {
    _check_params(argc, argv);
    char* service;
    _extract_params(argc, argv, &service);
    if (_start_server(service) == SERVER_ERROR) {
        return ERROR;
    }
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    return SUCCESS;
}

void _check_params(int argc, char* argv[]) {
    int count_commands = argc - 1;

    if (count_commands > MAX_ARGUMENTS ||
        count_commands < MIN_ARGUMENTS) {
        exit(ERROR);
    }
}

void _extract_params(int argc, char* argv[], char** service) {
    *service = argv[1];
}

int _start_server(char* service) {
    return start_server(service);
}
