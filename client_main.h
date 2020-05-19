// Copyright [2020] <Federico Jure>
#ifndef CLIENT_MAIN_H_
#define CLIENT_MAIN_H_

void _check_params(int argc, char* argv[]);

/*
Recibe los argumentos planos ingresados por el usuario y guarda
en los punteros address, service, entry_file, los respectivos 
valores extraidos.
*/
void _extract_params(int argc,
                    char* argv[],
                    char** address,
                    char** service,
                    FILE** entry_file);
int _start_client(char* address, char* service, FILE* entry_file);

#endif  // CLIENT_MAIN_H_
