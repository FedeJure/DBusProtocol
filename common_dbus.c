#include <string.h>
#include "./common_dbus.h"


int dbus_read_header(dbus_data_t* self, char* buffer) {
    return 0;
}

int dbus_read_static_header(dbus_data_t* self, char* buffer) {
    if (strlen(buffer) != dbus_get_static_size()) return 1;
    self->endianess = buffer[0];
    self->type = buffer[1];
    self->flag = buffer[2];
    self->version = buffer[3];
    self->body_length = (unsigned int) buffer[4];
    self->id = (unsigned int) buffer[4 + sizeof(int)];
    return 0;
}

int dbus_make_header(char* buffer, dbus_data_t* data) {
    return 0;
}

int dbus_get_static_size() {
    return 4 + 2 * sizeof(int);
}