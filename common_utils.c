// Copyright [2020] <Federico Jure>

#include <arpa/inet.h>
#include <string.h>
#include <byteswap.h>
#include "./common_utils.h"


__uint32_t force_little_endian(__uint32_t num) {
  __uint32_t aux = ntohl(num);
  return bswap_32(aux);
}

void save_decimal_as_bytes(char **dest, __uint32_t num) {
  memcpy(*dest, &num, sizeof(num));
}

__uint32_t get_int_from_char_array(char* array) {
  __uint32_t aux;
  memcpy(&aux, array, sizeof(aux));
  return aux;
}

int round_up_eigth(int to_round) {
  if (to_round % 8 == 0)
    return to_round;
  return (8 - to_round % 8) + to_round;
}
