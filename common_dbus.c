#include "./common_dbus.h"


void read_header(dbus_data_t* destination, char* buffer) {
    parameter_t parameter = {
        "type",
        "data_type",
        112
    };
    destination->body_length = 1;
    destination->endianess = 1;
    destination->flag = 0;
    destination->id = 1;
    destination->parameters = parameter;
}

void make_header(char* buffer, dbus_data_t* data) {
    // TODO
}