/**
 * @file varnum.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief variable size number utilities 
 * @version 0.1
 * @date 2021-02-07
 */
    /* includes */
#include "mcp/varnum.h" /* this */

    /* functions */
/**
 * @brief varnum parsing utilities
 */
int verify_varnum(const char *buf, size_t max_len, int type_max) {
  if(!max_len)
    return VARNUM_OVERRUN;
  int len =  1;
  for(; *(const unsigned char*) buf & 0x80; buf++, len++) {
    if(len == type_max)
      return VARNUM_INVALID;
    if((size_t) len == max_len)
      return VARNUM_OVERRUN;
  }
  return len;
}
int verify_varint(const char *buf, size_t max_len) {
  return verify_varnum(buf, max_len, 5);
}
int verify_varlong(const char *buf, size_t max_len) {
  return verify_varnum(buf, max_len, 10);
}

/**
 * @brief calculate varnum size for fixed integer
 */
size_t size_varint(uint32_t varint) {
  if(varint < (1 << 7))
    return 1;
  if(varint < (1 << 14))
    return 2;
  if(varint < (1 << 21))
    return 3;
  if(varint < (1 << 28))
    return 4;
  return 5;
}
size_t size_varlong(uint64_t varlong) {
  if(varlong < (1 << 7))
    return 1;
  if(varlong < (1 << 14))
    return 2;
  if(varlong < (1 << 21))
    return 3;
  if(varlong < (1 << 28))
    return 4;
  if(varlong < (1ULL << 35))
    return 5;
  if(varlong < (1ULL << 42))
    return 6;
  if(varlong < (1ULL << 49))
    return 7;
  if(varlong < (1ULL << 56))
    return 8;
  if(varlong < (1ULL << 63))
    return 9;
  return 10;
}