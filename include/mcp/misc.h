/**
 * @file misc.h
 * @author andersonarc (andersonarc@github.com)
 * @brief miscellanous utilities
 * @version 0.6
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
typedef char* string_t;

/**
 * @brief indicates a runtime-allocated memory value
 */
#define MALLOC

//todo move generics to other file (type.h) and rename this file to annotation.h
/**
 * @brief std::optional replacement
 */
#define mcp_generic_optional(type)      \
typedef struct type##_optional_t {  \
    type value;                     \
    bool has_value;                 \
} type##_optional_t;

/**
 * @brief std::vector replacement
 */
#define mcp_generic_vector(type)     \
typedef struct type##_vector_t { \
    type* data;  \
    size_t size; \
} type##_vector_t;

#endif /* MCP_MISC_H */
