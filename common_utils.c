// Copyright [2020] <Federico Jure>
#include "./common_utils.h"

__uint32_t betole(__uint32_t num) {
  num = ((num << 8) & 0xFF00FF00) | ((num >> 8) & 0xFF00FF);
  return (num << 16) | (num >> 16);
}

void save_decimal_as_bytes(char **dest, __uint32_t num) {
  (*dest)[0] = (num >> 24) & 0xFF;
  (*dest)[1] = (num >> 16) & 0xFF;
  (*dest)[2] = (num >> 8) & 0xFF;
  (*dest)[3] = num & 0xFF;
}
