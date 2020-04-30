// Copyright [2020] <Federico Jure>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./client.h"
#include "./signal.h"
#include "./client_main.h"

#define MAX_ARGUMENTS 3
#define MIN_ARGUMENTS 2

#define ERROR 1
#define SUCCESS 0

int main(int argc, char* argv[]) {
    _check_params(argc, argv);
    char* address;
    char* service;
    FILE* entry_file;
    _extract_params(argc, argv, &address, &service, &entry_file);
    return _start_client(address, service, entry_file);
}

void _check_params(int argc, char* argv[]) {
    int count_commands = argc - 1;

    if (count_commands > MAX_ARGUMENTS ||
        count_commands < MIN_ARGUMENTS) {
        exit(ERROR);
    }
}

void _extract_params(int argc, char* argv[], char** address, char** service,
                        FILE** entry_file) {
    *address = argv[1];
    *service = argv[2];
    if (argc - 1 == 3) {
        *entry_file = fopen(argv[3], "r");
        if (*entry_file == NULL) {
            exit(ERROR);
        }
    } else {
        *entry_file = stdin;
    }
}

int _start_client(char* address, char* service, FILE* entry_file) {
    return start_client(address, service, entry_file);
}
