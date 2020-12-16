/**
 * @file translationutils.h
 * @author andersonarc (andersonarc@github.com)
 * @brief utilities required for transition from C++ to C
 * @version 0.2
 * @date 2020-12-07
 */
      /* header guard */
#ifndef TRANSLATION_UTILS_H
#define TRANSLATION_UTILS_H

     /* includes */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

      /* defines */
/**
 * todo
 * @brief stub nbt tag
 */
#define nbt_tag_compound void*

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

     /* typedefs */
/**
 * todo
 * @brief stub nbt enum
 */
typedef enum nbt_tag_type {
  NBT_TAG_END,
  NBT_TAG_BYTE,
  NBT_TAG_SHORT,
  NBT_TAG_INT,
  NBT_TAG_LONG,
  NBT_TAG_FLOAT,
  NBT_TAG_DOUBLE,
  NBT_TAG_BYTE_ARRAY,
  NBT_TAG_STRING,
  NBT_TAG_LIST,
  NBT_TAG_COMPOUND,
  NBT_TAG_INT_ARRAY,
  NBT_TAG_LONG_ARRAY
} nbt_tag_type;

     /* functions */
/**
 * @brief exit with error
 * 
 * @param message message string
 */
void runtime_error(string_t message);

/**
 * todo
 * @brief nbt stubs 
 */
void nbt_encode_full() {}
void nbt_decode_full() {}
void nbt_read_string() {}

#endif /* TRANSLATION_UTILS_H */
