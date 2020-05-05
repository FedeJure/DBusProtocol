#include <string.h>
#include <stdio.h>
#include "./common_dbus.h"
#include "./common_socket.h"

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
        self->params[index].length = (unsigned int)buffer[4]; // TODO no toma en cuenta padding, reveer esto
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

void _dbus_build_stream(char*** params, unsigned int id) {
    char* stream = malloc(1);
    int method_params_count = _dbus_get_method_params_count((*params)[3]);
    char** signature = malloc( method_params_count * sizeof(char*));
    for (size_t i = 0; i < method_params_count; i++) { signature[i] = malloc(1); }
    
    char* method_name = malloc(1);
    _dbus_get_signature_method(params, &method_name, &signature, method_params_count);
    // static header
    stream = realloc(stream, 4 + 2 * sizeof(__uint32_t));
    stream[0] = 'l';
    stream[1] = 0x01;
    stream[2] = 0x0;
    stream[3] = 0x01;
    stream[4] =_dbus_get_body_length(params);
    // char* dest = *params[0];
    // char* path = *params[1];
    // char* interface = *params[2];
    // char* method = *params[3];

}

int _dbus_get_body_length(char*** params) {
    int length = 0;
    length += sizeof(__uint32_t);
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
        memcpy(*destination, *pointer, size);
        printf("%s\n",*destination);
}