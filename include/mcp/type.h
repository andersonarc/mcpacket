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
#include "mcp/particle.h"  /* particle types */
#include "mcp/misc.h"       /* miscellanous */
#include "mcp/nbt.h"        /* nbt data */

      /* generic typedefs */
vector_typedef(char)
vector_typedef(int32_t)
optional_typedef(mcp_type_NbtTagCompound)
optional_typedef(string_t)
optional_typedef(int32_t)

      /* typedefs */
/**
 * @brief minecraft uuid
 */
typedef struct mcp_type_UUID {
  uint64_t msb;
  uint64_t lsb;
} mcp_type_UUID;
optional_typedef(mcp_type_UUID)

/**
 * @brief minecraft position
 */
typedef struct mcp_type_Position {
  int32_t x;
  int32_t y;
  int32_t z;
} mcp_type_Position;
optional_typedef(mcp_type_Position)

/**
 * @brief minecraft container slot
 */
typedef struct mcp_type_Slot {
  uint8_t present;
  int32_t item_id;
  int8_t item_count;
  mcp_type_NbtTagCompound_optional_t nbt_data;
} mcp_type_Slot;
vector_typedef(mcp_type_Slot)

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
 * @brief minecraft tag
 */
typedef struct mcp_type_Tag {
  char* tag_name;
  int32_t_vector_t entries;
} mcp_type_Tag;
vector_typedef(mcp_type_Tag)

/**
 * @brief minecraft item
 */
typedef struct mcp_type_Item {
  int8_t slot;
  mcp_type_Slot item;
} mcp_type_Item;
vector_typedef(mcp_type_Item)

/**
 * @brief minecraft entity equipment
 */
typedef struct mcp_type_EntityEquipment {
   mcp_type_Item_vector_t equipments;
} mcp_type_EntityEquipment;

/**
 * @brief minecraft entity metadata
 */
typedef struct mcp_type_EntityMetadata {
  mcp_type_NbtTag* data;
} mcp_type_EntityMetadata;

#endif /* MCP_TYPE_H */