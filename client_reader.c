// Copyright [2020] <Federico Jure>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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

void reader_next_buffer_until_space(reader_t* self, char** buffer) {
    size_t size = self->bytes_to_read;
    *buffer = realloc(*buffer, size);
    memset(*buffer, 0, size);
    int index = 0;
    do {
        (*buffer)[index] = fgetc(self->file);
        if (strlen(*buffer) >= size) {
            size = size + self->bytes_to_read;
            *buffer = realloc(*buffer, size);
        }
        if ((*buffer)[index] == EOF) {
            self->reading = false;
        }
        index++;
        //*buffer[strlen(*buffer)] = readed;
    } while(_reader_stop_reading_line((*buffer)[index-1]) != true);
}

bool _reader_stop_reading_line(char c) {
    return c == EOF || c == '\n' || c == ' ';
}