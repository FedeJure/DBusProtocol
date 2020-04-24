#ifndef CLIENT_READER_H_
#define CLIENT_READER_H_
#include <stdio.h>

typedef struct reader_t {
    int reading;
    int bytes_to_read;
    FILE* file;
} reader_t;

void init_reader(reader_t* self, FILE* file, int bytes);
void reader_next_buffer(reader_t* self, char** buffer);

#endif  // CLIENT_READER_H_