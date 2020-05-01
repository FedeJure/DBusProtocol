#include <string.h>
#include <stdio.h>
#include "./common_dbus.h"
#include "./common_socket.h"

void dbus_init(dbus_data_t* self, char*** data) {
    for (size_t i = 0; i < 5; i++)
    {
        self->params[i].type = 0x00;
        self->params[i].length = 0;
        self->params[i].data_type = 0x00;

    }
    
    self->params_data = data;
    self->body_length = 0;
    self->endianess = 'l';
    self->flag = 0x00;
    self->id = 1;
    self->type = 0;
    self->version = 0;

}

int dbus_read_buffer(dbus_data_t* self, int client_fd) {
    _read_header_general_data(self, client_fd);
    _read_header_parameters(self, client_fd);
    _read_body_data(self, client_fd);
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
    self->params_count = ((unsigned int) buffer[4 + 2 * sizeof(int)]) / 
            (dbus_get_param_size() + dbus_get_static_size());
    return 0;
}

int _read_header_parameters(dbus_data_t* self, int client_fd) {
    size_t static_size_to_read = 8;
    char buffer[static_size_to_read];
    for (size_t i = 0; i < self->params_count ; i++)
    {
        memset(&buffer, 0, static_size_to_read);
        socket_read(client_fd, buffer, static_size_to_read);
        self->params[i].type = buffer[0];
        self->params[i].data_type = buffer[2];
        self->params[i].length = (unsigned int)buffer[4]; // TODO no toma en cuenta padding, reveer esto
        (*self->params_data)[i] = realloc((*self->params_data)[i], self->params[i].length);
        socket_read(client_fd, (*self->params_data)[i], self->params[i].length);    
    }
    
    return 0;
}

int _read_body_data(dbus_data_t* self, int client_fd) {
    return 0;
}

int dbus_make_header(dbus_data_t* self, char* buffer, unsigned int body_size,
                        unsigned int id, unsigned int params_count, dbus_params_t* params) {
    char endiannes = 'l';
    char type = 0x01;
    char flag = 0x00;
    char version = 0x01;

    buffer[0] = endiannes;
    buffer[1] = type;
    buffer[2] = flag;
    buffer[3] = version;
    buffer[4] = body_size;
    

    
    return 0;
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