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
#include "mcp/buffer.h" /* buffered io */

    /* defines */
/**
 * @brief stub nbt tag
 */
#define mcp_type_NbtTagCompound void*

    /* typedefs */
/**
 * @brief stub nbt enum
 */
typedef enum mcp_type_NbtTag {
  MCP_NBT_TAG_END,
  MCP_NBT_TAG_BYTE,
  MCP_NBT_TAG_SHORT,
  MCP_NBT_TAG_INT,
  MCP_NBT_TAG_LONG,
  MCP_NBT_TAG_FLOAT,
  MCP_NBT_TAG_DOUBLE,
  MCP_NBT_TAG_BYTE_ARRAY,
  MCP_NBT_TAG_STRING,
  MCP_NBT_TAG_LIST,
  MCP_NBT_TAG_COMPOUND,
  MCP_NBT_TAG_INT_ARRAY,
  MCP_NBT_TAG_LONG_ARRAY
} mcp_type_NbtTag;

    /* functions */
/**
 * @brief nbt stub functionsq
 */
void mcp_type_NbtTagCompound_encode(mcp_type_NbtTagCompound* this, buffer_t dest);
void mcp_type_NbtTagCompound_decode(mcp_type_NbtTagCompound* this, buffer_t src);
void mcp_type_NbtTagCompound_read(mcp_type_NbtTagCompound* this, buffer_t src);

#endif /* MCP_NBT_H */
