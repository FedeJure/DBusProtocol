// Copyright [2020] <Federico Jure>
#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_
#include <stdio.h>

#define END_OF_STRING 1

void save_decimal_as_bytes(char **dest, __uint32_t num);
int get_int_from_char_array(char* array);
__uint32_t betole(__uint32_t num);
int round_up_eigth(int to_round);
#endif  // COMMON_UTILS_H_
