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
    unsigned int array_length;
    unsigned int params_count;
    dbus_params_t params[MAX_PARAMS_COUNT];
    char*** params_data;
    char*** body_data;
} dbus_data_t;


int dbus_read_header(dbus_data_t* self, int client_fd);
int _read_header_general_data(dbus_data_t* self, int client_fd);
int _read_parameters(dbus_data_t* self, int client_fd);
int dbus_read_body(dbus_data_t* self,int client_fd);
int dbus_get_static_size();
int dbus_get_param_size();
void dbus_init(dbus_data_t* self, char*** data, char*** body_data);
int dbus_get_max_params_count();
int round_up_eigth(int to_round);
int _read_next_parameter(int client_fd, char* buffer, int size);
size_t _dbus_build_stream(char** stream, char*** params, unsigned int params_count, unsigned int id);
int _dbus_get_body_length(char*** params, int count);
int _dbus_get_header_length(char*** params, int count);
void _dbus_build_static_header(char** stream_chunk, int* stream_pointer, char*** signature, int method_params_count, __uint32_t id);
void _dbus_build_variable_header(char** stream_chunk, int* stream_pointer, char*** params, int params_count);
void _dbus_build_body(char** stream_chunk, int* stream_pointer, char*** signature, int method_params_count);
void _dbus_get_signature_method(char*** buffer, char** method_name, char*** signature, int params_count);
int _dbus_get_method_params_count(char* method);
void _dbus_read_until_separator(char** destination, char** pointer, char** rest, char* delim);

#endif  // COMMON_DBUS_H_