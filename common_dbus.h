// Copyright [2020] <Federico Jure>
#ifndef COMMON_DBUS_H_
#define COMMON_DBUS_H_

#define MAX_PARAMS_COUNT 5

typedef struct params {
    char type;
    char data_type;
    int length;
} dbus_params_t;

typedef struct dbus {
    char endianess;
    int type;
    int flag;
    unsigned int version;
    unsigned int body_length;
    unsigned int id;
    unsigned int params_count;
    dbus_params_t params[MAX_PARAMS_COUNT];
    char*** params_data;
} dbus_data_t;


int dbus_make_header(dbus_data_t* data, char* buffer, unsigned int body_size,
                        unsigned int id, unsigned int params_count, dbus_params_t* params);
int dbus_read_buffer(dbus_data_t* self, int client_fd);
int _read_header_general_data(dbus_data_t* self, int client_fd);
int _read_header_parameters(dbus_data_t* self, int client_fd);
int _read_body_data(dbus_data_t* self, int client_fd);
int dbus_get_static_size();
int dbus_get_param_size();
void dbus_init(dbus_data_t* self, char*** data);
int dbus_get_max_params_count();



#endif  // COMMON_DBUS_H_