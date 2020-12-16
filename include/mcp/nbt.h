/**
 * @file nbt.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief minecraft nbt tag parsing
 * @version 0.1
 * @date 2020-12-16
 */
    /* header guard */
#ifndef MCP_NBT_H
#define MCP_NBT_H

    /* includes */
#include <stdio.h> /* files */

    /* defines */
/**
 * todo
 * @brief stub nbt tag
 */
#define nbt_tag_compound void*

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
 * todo
 * @brief nbt stubs 
 */
void nbt_encode_full(FILE* dest, nbt_tag_compound* this);
void nbt_decode_full(FILE* src, nbt_tag_compound* this);
nbt_tag_compound nbt_read_string(FILE* src);

#endif /* MCP_NBT_H */
