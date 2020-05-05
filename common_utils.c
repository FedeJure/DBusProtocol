#include "common_utils.h"

__uint32_t betole( __uint32_t num ) {
    num = ((num << 8) & 0xFF00FF00 ) | ((num >> 8) & 0xFF00FF );
    return (num << 16) | (num >> 16);
}