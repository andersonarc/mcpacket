/**
 * @file varnum.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief variable size number utilities
 * @version 0.1
 * @date 2021-02-07
 */
    /* header guard */
#ifndef MCP_VARNUM_H
#define MCP_VARNUM_H

    /* includes */
#include <stdint.h> /* int types */
#include <stddef.h> /* size_t */

    /* typedefs */
/**
 * @brief varnum error enumeration
 */
typedef enum varnum_fail {
  VARNUM_INVALID = -1, /* impossible varnum, give up */
  VARNUM_OVERRUN = -2  /* your buffer is too small, read more */
} varnum_fail;

    /* functions */
/**
 * @brief varnum verification utilities
 */
int verify_varnum(const char *buf, size_t max_len, int type_max);
int verify_varint(const char *buf, size_t max_len);
int verify_varlong(const char *buf, size_t max_len);

/**
 * @brief calculate varnum size for fixed integer
 */
size_t size_varint(uint32_t varint);
size_t size_varlong(uint64_t varlong);

#endif /* MCP_VARNUM_H */