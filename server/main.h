// Copyright [2019] <Federico Jure>
#ifndef SERVER_MAIN_H_
#define SERVER_MAIN_H_
#endif  // SERVER_MAIN_H_
int _check_params(int argc, char* argv[]);
void _extract_params(int argc, char* argv[], char** service);
int _start_server(char* service);
