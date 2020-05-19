// Copyright [2020] <Federico Jure>
#ifndef CLIENT_READER_H_
#define CLIENT_READER_H_
#include <stdio.h>
#include <stdbool.h>

typedef struct reader_t {
    int reading;
    int bytes_to_read;
    FILE* file;
} reader_t;

void init_reader(reader_t* self, FILE* file, int bytes);

/*
Lee del archivo configurado en en tda reader, y guarda la siguiente
palabra encontrada en el buffer pasado por parametro
*/
void reader_next_buffer_until_space(reader_t* self, char** buffer);

#endif  // CLIENT_READER_H_
