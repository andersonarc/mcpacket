/**
 * @file types.h
 * @author SpockBotMC
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief data structures and encoders/decoders for minecraft data types
 * @version 0.4
 * @date 2020-12-13
 */
      /* header guard */
#ifndef MCP_TYPES_H
#define MCP_TYPES_H

      /* includes */
#include <stdint.h> /* integer types */
#include <string.h> /* string operations */ 
#include <endian.h> /* byte swap */ 
#include <malloc.h> /* memory allocation */ 
#include <unistd.h> /* socket io */

#include "mcp/particles.h"  /* particle types */
#include "mcp/misc.h"       /* misc */
#include "mcp/nbt.h"        /* nbt data */

      /* generic typedefs */
/**
 * @brief create predefined types for later use
 */
vector_typedef(char)
vector_typedef(int32_t)
optional_typedef(nbt_tag_compound)
optional_typedef(string_t)
optional_typedef(int32_t)

      /* minecraft data types, encoders and decoders */
/**
 * @brief minecraft UUID
 */
typedef struct mc_uuid {
  uint64_t msb;
  uint64_t lsb;
} mc_uuid;
optional_typedef(mc_uuid)
void enc_uuid(stream_t dest, mc_uuid src);
mc_uuid dec_uuid(stream_t src);

/**
 * @brief minecraft position
 */
typedef struct mc_position {
  int32_t x;
  int32_t y;
  int32_t z;
} mc_position;
optional_typedef(mc_position)
void enc_position(stream_t dest, mc_position src);
mc_position dec_position(stream_t src);

/**
 * @brief minecraft container slot
 */
typedef struct mc_slot {
  uint8_t present;
  int32_t item_id;
  int8_t item_count;
  nbt_tag_compound_optional_t nbt_data;
} mc_slot;
vector_typedef(mc_slot)
void mc_slot_encode(stream_t dest, mc_slot* this);
void mc_slot_decode(stream_t src, mc_slot* this);

/**
 * @brief minecraft particle
 */
typedef struct mc_particle {
  mc_particle_type type;
  int32_t block_state;
  float red;
  float green;
  float blue;
  float scale;
  mc_slot item;
} mc_particle;
void mc_particle_encode(stream_t dest, mc_particle* this);
void mc_particle_decode(stream_t src, mc_particle* this, mc_particle_type p_type);

/**
 * @brief minecraft item smelting
 */
typedef struct mc_smelting {
  char* group;
  mc_slot_vector_t ingredient;
  mc_slot result;
  float experience;
  int32_t cook_time;
} mc_smelting;
void mc_smelting_encode(stream_t dest, mc_smelting* this);
void mc_smelting_decode(stream_t src, mc_smelting* this);

/**
 * @brief minecraft tag
 */
typedef struct mc_tag {
  char* tag_name;
  int32_t_vector_t entries;
} mc_tag;
vector_typedef(mc_tag)
void mc_tag_encode(stream_t dest, mc_tag* this);
void mc_tag_decode(stream_t src, mc_tag* this);

/**
 * @brief minecraft item
 */
typedef struct mc_item {
  int8_t slot;
  mc_slot item;
} mc_item;
vector_typedef(mc_item)

/**
 * @brief minecraft entity equipment
 */
typedef struct mc_entity_equipment {
   mc_item_vector_t equipments;
} mc_entity_equipment;
void mc_entity_equipment_encode(stream_t dest, mc_entity_equipment* this);
void mc_entity_equipment_decode(stream_t src, mc_entity_equipment* this);

/**
 * @brief minecraft entity metadata
 */
typedef struct mc_entity_metadata {
  nbt_tag_type* data;
} mc_entity_metadata;
void mc_entity_metadata_encode(stream_t dest, mc_entity_metadata* this);
void mc_entity_metadata_decode(stream_t src, mc_entity_metadata* this);


      /* primitive type encoders and decoders */
/**
 * @brief uint8
 */
void enc_byte(stream_t dest, const uint8_t src);
uint8_t dec_byte(stream_t src);

/**
 * @brief big endian uint16
 */
void enc_be16(stream_t dest, uint16_t src);
uint16_t dec_be16(stream_t src);

/**
 * @brief little endian uint16
 */
void enc_le16(stream_t dest, uint16_t src);
uint16_t dec_le16(stream_t src);

/**
 * @brief big endian uint32
 */
void enc_be32(stream_t dest, uint32_t src);
uint32_t dec_be32(stream_t src);

/**
 * @brief little endian uint32
 */
void enc_le32(stream_t dest, uint32_t src);
uint32_t dec_le32(stream_t src);

/**
 * @brief big endian uint64
 */
void enc_be64(stream_t dest, uint64_t src);
uint64_t dec_be64(stream_t src);

/**
 * @brief little endian uint64
 */
void enc_le64(stream_t dest, uint64_t src);
uint64_t dec_le64(stream_t src);

/**
 * @brief big endian float32
 */
void enc_bef32(stream_t dest, float src);
float dec_bef32(stream_t src);

/**
 * @brief little endian float32
 */
void enc_lef32(stream_t dest, float src);
float dec_lef32(stream_t src);

/**
 * @brief big endian float64
 */
void enc_bef64(stream_t dest, double src);
double dec_bef64(stream_t src);

/**
 * @brief little endian float64
 */
void enc_lef64(stream_t dest, double src);
double dec_lef64(stream_t src);

/**
 * @brief varnum error enumeration
 */
typedef enum varnum_fail {
  VARNUM_INVALID = -1, /* impossible varnum, give up */
  VARNUM_OVERRUN = -2  /* your buffer is too small, read more */
} varnum_fail;

/**
 * @brief varnum parsing utilities
 */
int verify_varnum(const char *buf, size_t max_len, int type_max);
int verify_varint(const char *buf, size_t max_len);
int verify_varlong(const char *buf, size_t max_len);

/**
 * @brief get varnum size
 */
size_t size_varint(uint32_t varint);
size_t size_varlong(uint64_t varlong);

/**
 * @brief variable size integer
 */
void enc_varint(stream_t dest, uint64_t src);
int64_t dec_varint(stream_t src);

/**
 * @brief string
 */
void enc_string(stream_t dest, const char* src);
char* dec_string(stream_t src);

/**
 * @brief buffer
 */

void enc_buffer(stream_t dest, char_vector_t src);
char_vector_t dec_buffer(stream_t src, size_t len);

#endif /* MCP_TYPES_H */
