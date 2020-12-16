/**
 * @file data_utils.h
 * @author SpockBotMC
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief data structures and encoders/decoders for minecraft data types
 * @version 0.2
 * @date 2020-12-13
 */
      /* header guard */
#ifndef DATA_UTILS_H
#define DATA_UTILS_H

      /* includes */
#include <stdint.h> /* integer types */
#include <stdio.h>  /* file i/o */
#include <string.h> /* string operations */ .
#include <endian.h> /* byte swap */ .
#include <malloc.h> /* memory allocation */ .

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

      /* minecraft data types, encoders, decoders */
/**
 * @brief minecraft UUID type and encoder/decoder
 */
typedef struct mc_uuid {
  uint64_t msb;
  uint64_t lsb;
} mc_uuid;
optional_typedef(mc_uuid)
void enc_uuid(FILE* dest, mc_uuid src);
mc_uuid dec_uuid(FILE* src);

/**
 * @brief minecraft position type
 */
typedef struct mc_position {
  int32_t x;
  int32_t y;
  int32_t z;
} mc_position;
optional_typedef(mc_position)
void enc_position(FILE* dest, mc_position src);
mc_position dec_position(FILE* src);

/**
 * @brief minecraft slot type and encoder/decoder
 */
typedef struct mc_slot {
  uint8_t present;
  int32_t item_id;
  int8_t item_count;
  nbt_tag_compound_optional_t nbt_data;
} mc_slot;
vector_typedef(mc_slot)
void mc_slot_encode(FILE* dest, mc_slot* this);
void mc_slot_decode(FILE* src, mc_slot* this);

/**
 * @brief minecraft particle type and encoder/decoder
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
void mc_particle_encode(FILE* dest, mc_particle* this);
void mc_particle_decode(FILE* src, mc_particle* this, mc_particle_type p_type);

/**
 * @brief minecraft smelting type and encoder/decoder
 */
typedef struct mc_smelting {
  char* group;
  mc_slot_vector_t ingredient;
  mc_slot result;
  float experience;
  int32_t cook_time;
} mc_smelting;
void mc_smelting_encode(FILE* dest, mc_smelting* this);
void mc_smelting_decode(FILE* src, mc_smelting* this);

/**
 * @brief minecraft tag type and encoder/decoder
 */
typedef struct mc_tag {
  char* tag_name;
  int32_t_vector_t entries;
} mc_tag;
vector_typedef(mc_tag)
void mc_tag_encode(FILE* dest, mc_tag* this);
void mc_tag_decode(FILE* src, mc_tag* this);

/**
 * @brief minecraft item type
 */
typedef struct mc_item {
  int8_t slot;
  mc_slot item;
} mc_item;
vector_typedef(mc_item)

/**
 * @brief minecraft entyty equipment type and encoder/decoder
 */
typedef struct mc_entity_equipment {
   mc_item_vector_t equipments;
} mc_entity_equipment;
void mc_entity_equipment_encode(FILE* dest, mc_entity_equipment* this);
void mc_entity_equipment_decode(FILE* src, mc_entity_equipment* this);

/**
 * @brief minecraft entity metadata type and encoder/decoder
 */
typedef struct mc_entity_metadata {
  nbt_metadata_tag* data;
} mc_entity_metadata;
void mc_entity_metadata_encode(FILE* dest, mc_entity_metadata* this);
void mc_entity_metadata_decode(FILE* src, mc_entity_metadata* this);

/**
 * @brief uint8 encoder/decoder
 */
void enc_byte(FILE* dest, const uint8_t src);
uint8_t dec_byte(FILE* src);

/**
 * @brief big endian uint16 encoder/decoder
 */
void enc_be16(FILE* dest, uint16_t src);
uint16_t dec_be16(FILE* src);

/**
 * @brief little endian uint16 encoder/decoder
 */
void enc_le16(FILE* dest, uint16_t src);
uint16_t dec_le16(FILE* src);

/**
 * @brief big endian uint32 encoder/decoder
 */
void enc_be32(FILE* dest, uint32_t src);
uint32_t dec_be32(FILE* src);

/**
 * @brief little endian uint32 encoder/decoder
 */
void enc_le32(FILE* dest, uint32_t src);
uint32_t dec_le32(FILE* src);

/**
 * @brief big endian uint64 encoder/decoder
 */
void enc_be64(FILE* dest, uint64_t src);
uint64_t dec_be64(FILE* src);

/**
 * @brief little endian uint64 encoder/decoder
 */
void enc_le64(FILE* dest, uint64_t src);
uint64_t dec_le64(FILE* src);

/**
 * @brief big endian float32 encoder/decoder
 */
void enc_bef32(FILE* dest, float src);
float dec_bef32(FILE* src);

/**
 * @brief little endian float32 encoder/decoder
 */
void enc_lef32(FILE* dest, float src);
float dec_lef32(FILE* src);

/**
 * @brief big endian float64 encoder/decoder
 */
void enc_bef64(FILE* dest, double src);
double dec_bef64(FILE* src);

/**
 * @brief little endian float64 encoder/decoder
 */
void enc_lef64(FILE* dest, double src);
double dec_lef64(FILE* src);

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
 * @brief varint encoder/decoder
 */
void enc_varint(FILE* dest, uint64_t src);
int64_t dec_varint(FILE* src);

/**
 * @brief string encoder/decoder
 */
void enc_string(FILE* dest, const char* src);
char* dec_string(FILE* src);

/**
 * @brief buffer encoder/decoder
 */

void enc_buffer(FILE* dest, char_vector_t src);
char_vector_t dec_buffer(FILE* src, size_t len);

#endif /* DATA_UTILS_H */
