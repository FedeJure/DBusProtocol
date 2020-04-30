// Copyright [2020] <Federico Jure>
#ifndef COMMON_DBUS_H_
#define COMMON_DBUS_H_

typedef struct dbus {
    char endianess;
    int type;
    int flag;
    unsigned int version;
    unsigned int body_length;
    unsigned int id;
} dbus_data_t;


int dbus_read_header(dbus_data_t* destination, char* buffer);
int dbus_make_header(char* buffer, dbus_data_t* data);
int dbus_read_static_header(dbus_data_t* self, char* buffer);
int dbus_get_static_size();


#endif  // COMMON_DBUS_H_