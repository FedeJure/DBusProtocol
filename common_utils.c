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
  // (*dest)[0] = (num >> 24) & 0xFF;
  // (*dest)[1] = (num >> 16) & 0xFF;
  // (*dest)[2] = (num >> 8) & 0xFF;
  // (*dest)[3] = num & 0xFF;
  memcpy(*dest, &num, sizeof(num));
}

int get_int_from_char_array(char* array) {
  return ((array[3] << 24) | (array[2] << 16) |
          (array[1] << 8) | array[0]) & 0xFF;
}

int round_up_eigth(int to_round) {
  if (to_round % 8 == 0)
    return to_round;
  return (8 - to_round % 8) + to_round;
}
