/**
 * @file misc.h
 * @author andersonarc (andersonarc@github.com)
 * @brief utilities required for transition from C++ to C
 * @version 0.3
 * @date 2020-12-07
 */
      /* header guard */
#ifndef MCP_MISC_H
#define MCP_MISC_H

     /* includes */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

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

     /* functions */
/**
 * @brief exit with error
 * 
 * @param message message string
 */
void runtime_error(string_t message);

#endif /* MCP_MISC_H */
