/**
 * @file type.h
 * @author SpockBotMC
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief type definitions for minecraft data types
 * @version 0.6
 * @date 2020-12-13
 */
      /* header guard */
#ifndef MCP_TYPE_H
#define MCP_TYPE_H

      /* includes */
#include <stdint.h>         /* integer types */
#include "mcp/particle.h"   /* particle types */
#include "mcp/misc.h"       /* miscellanous */

      /* generic typedefs */
mcp_generic_vector(char)
mcp_generic_vector(int32_t)
mcp_generic_optional(string_t)
mcp_generic_optional(int32_t)

      /* typedefs */
/**
 * @brief minecraft uuid
 */
typedef struct mcp_type_UUID {
  uint64_t msb;
  uint64_t lsb;
} mcp_type_UUID;
mcp_generic_optional(mcp_type_UUID)

/**
 * @brief minecraft position
 */
typedef struct mcp_type_Position {
  int32_t x;
  int32_t y;
  int32_t z;
} mcp_type_Position;
mcp_generic_optional(mcp_type_Position)

/**
 * @brief minecraft tag
 */
typedef struct mcp_type_Tag {
  char* tag_name;
  int32_t_vector_t entries;
} mcp_type_Tag;
mcp_generic_vector(mcp_type_Tag)

/**
 * @brief nbt tag stub
 */
typedef void* mcp_type_NbtTagCompound;
mcp_generic_optional(mcp_type_NbtTagCompound)

/**
 * @brief nbt type enum
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

/**
 * @brief minecraft entity metadata
 */
typedef struct mcp_type_EntityMetadata {
  mcp_type_NbtTag* data;
} mcp_type_EntityMetadata;

/**
 * @brief minecraft container slot
 */
typedef struct mcp_type_Slot {
  uint8_t present;
  int32_t item_id;
  int8_t item_count;
  mcp_type_NbtTagCompound_optional_t nbt_data;
} mcp_type_Slot;
mcp_generic_vector(mcp_type_Slot)

/**
 * @brief minecraft particle
 */
typedef struct mcp_type_Particle {
  mcp_type_ParticleType type;
  int32_t block_state;
  float red;
  float green;
  float blue;
  float scale;
  mcp_type_Slot item;
} mcp_type_Particle;

/**
 * @brief minecraft item smelting
 */
typedef struct mcp_type_Smelting {
  char* group;
  mcp_type_Slot_vector_t ingredient;
  mcp_type_Slot result;
  float experience;
  int32_t cook_time;
} mcp_type_Smelting;

/**
 * @brief minecraft item
 */
typedef struct mcp_type_Item {
  int8_t slot;
  mcp_type_Slot item;
} mcp_type_Item;
mcp_generic_vector(mcp_type_Item)

/**
 * @brief minecraft entity equipment
 */
typedef struct mcp_type_EntityEquipment {
   mcp_type_Item_vector_t equipments;
} mcp_type_EntityEquipment;

#endif /* MCP_TYPE_H */