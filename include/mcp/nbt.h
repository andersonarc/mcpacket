/**
 * @file nbt.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief minecraft nbt tag parsing
 * @todo this file is a stub
 * @version 0.2
 * @date 2020-12-16
 */
    /* header guard */
#ifndef MCP_NBT_H
#define MCP_NBT_H

    /* includes */
#include "mcp/stream.h" /* stream io */

    /* defines */
/**
 * @brief stub nbt tag
 */
#define nbt_tag_compound void*

    /* typedefs */
/**
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
 * @brief nbt stub functions
 */
void nbt_encode_full(stream_t dest, nbt_tag_compound* this);
void nbt_decode_full(stream_t src, nbt_tag_compound* this);
nbt_tag_compound nbt_read_string(stream_t src);

#endif /* MCP_NBT_H */
