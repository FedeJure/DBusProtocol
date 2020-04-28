// Copyright [2019] <Federico Jure>
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
void reader_next_buffer_in_same_line(reader_t* self, char* buffer, bool* line_break);

#endif  // CLIENT_READER_H_
