// Copyright [2020] <Federico Jure>
#ifndef COMMON_DBUS_H_
#define COMMON_DBUS_H_

typedef struct parameter {
    char* type;
    char* data_type;
    unsigned int data_length;

} parameter_t;

typedef struct dbus {
    char endianess;
    int type;
    int flag;
    unsigned int version;
    unsigned int body_length;
    unsigned int id; 
    parameter_t parameters;
} dbus_data_t;


void read_header(dbus_data_t* destination, char* buffer);
void make_header(char* buffer, dbus_data_t* data);

#endif  // COMMON_DBUS_H_