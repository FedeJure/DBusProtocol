#include <string.h>
#include <stdio.h>
#include "common_dbus.h"
#include "common_socket.h"
#include "common_utils.h"

void dbus_init(dbus_data_t* self, char*** data, char*** body_data) {
    for (size_t i = 0; i < MAX_PARAMS_COUNT; i++)
    {
        self->params[i].type = 0x00;
        self->params[i].length = 0;
        self->params[i].data_type = 0x00;

    }
    self->body_data = body_data;
    self->params_data = data;
    self->params_count = 0;
    self->body_length = 0;
    self->endianess = 'l';
    self->flag = 0x00;
    self->id = 1;
    self->type = 0;
    self->version = 0;

}

int dbus_read_header(dbus_data_t* self, int client_fd) {
    _read_header_general_data(self, client_fd);
    _read_parameters(self, client_fd);
    return 0;
}

int _read_header_general_data(dbus_data_t* self, int client_fd) {
    char buffer[dbus_get_static_size()];
    socket_read(client_fd, buffer, dbus_get_static_size());
    self->endianess = buffer[0];
    self->type = buffer[1];
    self->flag = buffer[2];
    self->version = buffer[3];
    self->body_length = (unsigned int) buffer[4];
    self->id = (unsigned int) buffer[4 + sizeof(int)];
    self->array_length = ((unsigned int) buffer[4 + 2 * sizeof(int)]);
    return 0;
}

int _read_parameters(dbus_data_t* self, int client_fd) {
    size_t static_size_to_read = 8;    
    char buffer[static_size_to_read];
    int index = 0;
    int bytes_readed = 0;
    while (bytes_readed < round_up_eigth(self->array_length)) {
        bytes_readed += socket_read(client_fd, buffer, static_size_to_read);
        self->params[index].type = buffer[0];
        self->params[index].data_type = buffer[2];
        self->params[index].length = (unsigned int)buffer[4];
        self->params_count++;
        if (bytes_readed >= round_up_eigth(self->array_length)) { break; }
        int rounded_length = round_up_eigth(self->params[index].length + 1);
        (*self->params_data)[index] = realloc((*self->params_data)[index], rounded_length);
        bytes_readed += socket_read(client_fd, (*self->params_data)[index], rounded_length);  
        ++index;
    };
    return 0;
}

int dbus_read_body(dbus_data_t* self,int client_fd) {
    size_t static_size_to_read = 4;
    char buffer[static_size_to_read];
    int bytes_readed = 0;
    int index = 0;
    while (bytes_readed < self->body_length) {
        bytes_readed += round_up_eigth(socket_read(client_fd, buffer, static_size_to_read));
        printf("%s\n", buffer);
        unsigned int length = buffer[0]; 
        (*self->body_data)[index] = realloc((*self->body_data)[index], length + 1);
        bytes_readed += round_up_eigth(socket_read(client_fd, (*self->body_data)[index], length + 1));
        index++;
    }
    return 0;
}

int _read_next_parameter(int client_fd, char* buffer, int size) {
    memset(&buffer, 0, size);
    return socket_read(client_fd, buffer, size);

}

int dbus_get_static_size() {
    return 4 + 3 * sizeof(int);
}

int dbus_get_param_size() {
    return 8;
}

int dbus_get_max_params_count() {
    return MAX_PARAMS_COUNT;
}

int round_up_eigth(int to_round) {
        if (to_round % 8 == 0) return to_round;
        return (8 - to_round % 8) + to_round;
}

void _dbus_build_stream(char*** params, unsigned int params_count, unsigned int id) {
    int method_params_count = _dbus_get_method_params_count((*params)[3]);
    char** signature = malloc( method_params_count * sizeof(char*));
    for (size_t i = 0; i < method_params_count; i++) { signature[i] = malloc(1); }
    
    char* method_name = malloc(1);
    _dbus_get_signature_method(params, &method_name, &signature, method_params_count);

    size_t static_size = 4 + 2 * sizeof(__uint32_t);
    size_t stream_size = static_size +
                        _dbus_get_body_length(&signature, method_params_count) + sizeof(__uint32_t) +
                        _dbus_get_header_length(params, params_count) + sizeof(__uint32_t);
    int stream_pointer = 0;
    char* stream_chunk = malloc(stream_size);
    memset(stream_chunk, 0, stream_size);

    _dbus_build_static_header(&stream_chunk, &stream_pointer, &signature, method_params_count, id);
    stream_pointer++;
    _dbus_build_variable_header(&stream_chunk, &stream_pointer, params, params_count);
    stream_pointer++;
    _dbus_build_body(&stream_chunk, &stream_pointer, &signature, method_params_count);

    
    for (size_t i = 0; i < method_params_count; i++) { free(signature[i]); }
    free(signature);
    free(stream_chunk);
    free(method_name);
}

void _dbus_build_static_header(char** stream_chunk, int* stream_pointer, char*** signature, int method_params_count, __uint32_t id) {
    
    (*stream_chunk)[(*stream_pointer)++] = 'l';
    (*stream_chunk)[(*stream_pointer)++] = 0x01;
    (*stream_chunk)[(*stream_pointer)++] = 0x0;
    (*stream_chunk)[(*stream_pointer)++] = 0x01;
    (*stream_chunk)[*stream_pointer] =  betole(_dbus_get_body_length(signature, method_params_count));
    (*stream_chunk)[*stream_pointer += sizeof(__uint32_t)] = betole(id);
}

void _dbus_build_variable_header(char** stream_chunk, int* stream_pointer, char*** params, int params_count) {
    (*stream_chunk)[(*stream_pointer)++] = betole(_dbus_get_header_length(params, params_count));
    char params_types[4] = { 0x6, 0x1, 0x2, 0x3};
    char params_data_types[4] = { 's', 'o', 's', 's'};
    for (size_t i = 0; i < params_count; i++) {
        (*stream_chunk)[(*stream_pointer)++] = params_types[i];
        (*stream_chunk)[(*stream_pointer)++] = 0x1;
        (*stream_chunk)[(*stream_pointer)++] = params_data_types[i];
        (*stream_chunk)[(*stream_pointer)] = strlen((*params)[i]);
        (*stream_pointer) += sizeof(__uint32_t);
        memcpy((*stream_chunk) + (*stream_pointer), (*params)[i], strlen((*params)[i]) + 1);
        (*stream_pointer) += i == (params_count - 1) ? 
                                strlen((*params)[i]) :
                                round_up_eigth(strlen((*params)[i]));
    }
}

void _dbus_build_body(char** stream_chunk, int* stream_pointer, char*** signature, int method_params_count) {
    for (size_t i = 0; i < method_params_count; i++) {
        printf("\n%ld\n", i);
        (*stream_chunk)[(*stream_pointer)] = strlen((*signature)[i]);
        memcpy((*stream_chunk) + (*stream_pointer), (*signature)[i], strlen((*signature)[i]) + 1);
        (*stream_pointer) += (i == method_params_count - 1) ? 
                            strlen((*signature)[i]) + 1 :
                            round_up_eigth(strlen((*signature)[i]) + 1);
    }
    
}

int _dbus_get_body_length(char*** signature, int count) {
    size_t length = 0;
    for (size_t i = 0; i < count - 1; i++) {
        length += sizeof(__uint32_t);
        length += round_up_eigth(strlen((*signature)[i]) + 1);
    }
    length += strlen((*signature)[count - 1]) + 1;
    return length;
}

int _dbus_get_header_length(char*** params, int count) {
    size_t length = 0;
    length += sizeof(__uint32_t);
    for (size_t i = 0; i < count - 1; i++) {
        length += round_up_eigth(sizeof(__uint32_t) + strlen((*params)[i]) + 1);
    }
    length += sizeof(__uint32_t) + strlen((*params)[count - 1]) + 1;
    return length;
}

void _dbus_get_signature_method(char*** buffer, char** method_name, char*** signature, int params_count) {
    char* rest = (*buffer)[3];
    char* actual;
    _dbus_read_until_separator(method_name, &actual, &rest, "(");
    char** param;
    for (size_t i = 0; i < params_count - 1; i++) { 
        param = &(*signature)[i];
        _dbus_read_until_separator(param, &actual, &rest, ",");
    }
    param = &((*signature)[params_count - 1]);
    _dbus_read_until_separator(param, &actual, &rest, ")");
}

int _dbus_get_method_params_count(char* method) {
    int comma_count = 0;
    for (size_t i = 0; i < strlen(method); i++) {
        comma_count += method[i] == ',' ? 1 : 0;
    }
    return comma_count + 1;
    
}

void _dbus_read_until_separator(char** destination, char** pointer, char** rest, char* delim) {
        *pointer = __strtok_r(*rest, delim, rest);
        size_t size = *rest - *pointer;
        *destination = realloc(*destination, size);
        memset(*destination, 0, size);
        memcpy(*destination, *pointer, size);
}
