// Copyright [2020] <Federico Jure>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./client_reader.h"

#define READING 1
#define NOT_READING 0

void init_reader(reader_t* self, FILE* file, int bytes) {
    self->bytes_to_read = bytes;
    self->file = file;
    self->reading = true;
}


void reader_next_buffer_in_same_line(reader_t* self, char* buffer, bool* line_break) {
    for (size_t i = 0; i < self->bytes_to_read; i++) {
        char readed = fgetc(self->file);
        if (readed == '\n') {
            *line_break = true;
            buffer[i] = EOF;
        }
        if (readed == EOF) {
            self->reading = false;
            *line_break = true;
        }
        if (*line_break == false && self->reading == true) {
            buffer[i] = readed;
        }
    }
}
