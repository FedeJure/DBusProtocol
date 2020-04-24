#include <stdio.h>
#include <stdlib.h>
#include "./client_reader.h"

#define READING 1
#define NOT_READING 0

void init_reader(reader_t* self, FILE* file, int bytes) {
    self->bytes_to_read = bytes;
    self->file = file;
    self->reading = READING;
}


void reader_next_buffer(reader_t* self, char** buffer) {
    for (size_t i = 0; i < self->bytes_to_read; i++)
    {
        *buffer[i] = fgetc(self->file);
        if (*buffer[i] == EOF) {
            self->reading = 0;
        }
    }
}