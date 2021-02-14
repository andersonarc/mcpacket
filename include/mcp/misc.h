/**
 * @file misc.h
 * @author andersonarc (andersonarc@github.com)
 * @brief miscellanous utilities
 * @version 0.5
 * @date 2020-12-07
 */
      /* header guard */
#ifndef MCP_MISC_H
#define MCP_MISC_H

     /* includes */
#include <stddef.h>  /* size_t */
#include <stdbool.h> /* boolean */

     /* defines */
/**
 * @brief for better readability in io functions
 */
#define SINGLE_BYTE sizeof(char)

/**
 * @brief string type
 */
#define string_t char*

/**
 * @brief indicates a runtime-allocated memory value
 */
#define MALLOC

/**
 * @brief std::optional replacement
 */
#define optional_typedef(type)      \
typedef struct type##_optional_t {  \
    type value;                     \
    bool has_value;                 \
} type##_optional_t;

/**
 * @brief std::vector replacement
 */
#define vector_typedef(type)     \
typedef struct type##_vector_t { \
    type* data;  \
    size_t size; \
} type##_vector_t;

#endif /* MCP_MISC_H */
