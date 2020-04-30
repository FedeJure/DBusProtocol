// Copyright [2020] <Federico Jure>
#ifndef SERVER_MAIN_H_
#define SERVER_MAIN_H_

void _check_params(int argc, char* argv[]);
void _extract_params(int argc, char* argv[], char** service);
int _start_server(char* service);

#endif  // SERVER_MAIN_H_
