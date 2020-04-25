// Copyright [2019] <Federico Jure>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./client_reader.h"

#define READING 1
#define NOT_READING 0

void init_reader(reader_t* self, FILE* file, int bytes) {
    self->bytes_to_read = bytes;
    self->file = file;
    self->reading = READING;
}


void reader_next_buffer_in_same_line(reader_t* self, char* buffer) {
    bool in_line = true;
    for (size_t i = 0; i < self->bytes_to_read; i++) {
        char readed = in_line ? fgetc(self->file) : ' ';
        if (readed == '\n') {
            in_line = false;
            readed = ' ';
        }
        buffer[i] = readed;
        if (readed == EOF) {
            self->reading = 0;
        }
    }
}
