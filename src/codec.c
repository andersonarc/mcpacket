/**
 * @file codec.c
 * @author SpockBotMC
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief encoders/decoders for minecraft data types
 * @version 0.6
 * @date 2020-12-14
 */
      /* includes */
#include "mcp/codec.h"     /* this */
#include "csafe/assertd.h" /* debug assertions */
#include <stdlib.h>        /* memory allocation */ 
#include <string.h>        /* string operations */ 

      /* functions */
/**
 * minecraft uuid
 */
void mcp_encode_type_UUID(mcp_type_UUID* this, mcp_buffer_t* dest) {
  mcp_encode_be64(this->msb, dest);
  mcp_encode_be64(this->lsb, dest);
}
void mcp_decode_type_UUID(mcp_type_UUID* this, mcp_buffer_t* src) {
  mcp_decode_be64(&this->msb, src);
  mcp_decode_be64(&this->lsb, src);
}

/**
 * minecraft container slot
 */
void mcp_encode_type_Slot(mcp_type_Slot* this, mcp_buffer_t* dest) {
  mcp_encode_byte(this->present, dest);
  if (this->present) {
    mcp_encode_varint(this->item_id, dest);
    mcp_encode_byte(this->item_count, dest);
    if(this->nbt_data.has_value) {
      mcp_encode_type_NbtTagCompound(this->nbt_data.value, dest);
    } else {
      mcp_encode_byte(MCP_NBT_TAG_END, dest);
    }
  }
}
void mcp_decode_type_Slot(mcp_type_Slot* this, mcp_buffer_t* src) {
  mcp_decode_byte(&this->present, src);
  if (this->present) {
    this->item_id = mcp_decode_varint(src);
    mcp_decode_byte((uint8_t*) &this->item_count, src);
    uint8_t tag;
    mcp_decode_byte(&tag, src);
    if (tag == MCP_NBT_TAG_COMPOUND) {
      this->nbt_data.has_value = true;
      mcp_read_type_NbtTagCompound(&this->nbt_data.value, src);
    }
  }
}
void mcp_length_type_Slot(mcp_type_Slot* this, size_t* length) {
  *length += sizeof(this->present);
  if (this->present) {
    *length += mcp_length_varint(this->item_id);
    *length += sizeof(this->item_count);
    if(this->nbt_data.has_value) {
      *length += sizeof(this->nbt_data.value); /* todo nbt length */
    } else {
      *length += sizeof(char);
    }
  }
}

/**
 * minecraft particle
 */
void mcp_encode_type_Particle(mcp_type_Particle* this, mcp_buffer_t* dest) {
  switch(this->type) {
    case MCP_DEFINED_PARTICLE_BLOCK_DUST:
    case MCP_DEFINED_PARTICLE_FALLING_DUST:
      mcp_encode_varint(this->block_state, dest);
      break;

    case MCP_DEFINED_PARTICLE_DUST:
      mcp_encode_bef32(this->red, dest);
      mcp_encode_bef32(this->green, dest);
      mcp_encode_bef32(this->blue, dest);
      mcp_encode_bef32(this->scale, dest);
      break;

    case MCP_DEFINED_PARTICLE_ITEM:
      mcp_encode_type_Slot(&this->item, dest);
      break;

    default:
      break;
  }
}
void mcp_decode_type_Particle(mcp_type_Particle* this, mcp_type_ParticleType p_type, mcp_buffer_t* src) {
  this->type = p_type;
  switch (this->type) {
    case MCP_DEFINED_PARTICLE_BLOCK_DUST:
    case MCP_DEFINED_PARTICLE_FALLING_DUST:
      this->block_state = mcp_decode_varint(src);
      break;

    case MCP_DEFINED_PARTICLE_DUST:
      mcp_decode_be32((uint32_t*) &this->red, src);
      mcp_decode_be32((uint32_t*) &this->green, src);
      mcp_decode_be32((uint32_t*) &this->blue, src);
      mcp_decode_be32((uint32_t*) &this->scale, src);
      break;

    case MCP_DEFINED_PARTICLE_ITEM:
      mcp_decode_type_Slot(&this->item, src);
      break;

    default:
      break;
  }
}
void mcp_length_type_Particle(mcp_type_Particle* this, size_t* length) {
  switch (this->type) {
    case MCP_DEFINED_PARTICLE_BLOCK_DUST:
    case MCP_DEFINED_PARTICLE_FALLING_DUST:
      *length += mcp_length_varint(this->block_state);
      break;

    case MCP_DEFINED_PARTICLE_DUST:
      *length += sizeof(this->red);
      *length += sizeof(this->green);
      *length += sizeof(this->blue);
      *length += sizeof(this->scale);
      break;

    case MCP_DEFINED_PARTICLE_ITEM:
      mcp_length_type_Slot(&this->item, length);
      break;

    default:
      break;
  }
}

/**
 * minecraft item smelting
 */
void mcp_encode_type_Smelting(mcp_type_Smelting* this, mcp_buffer_t* dest) {
  mcp_encode_string(this->group, dest);
  mcp_encode_varint(this->ingredient.size, dest);
  for (size_t i = 0; i < this->ingredient.size; i++) {
    mcp_encode_type_Slot(&this->ingredient.data[i], dest);
  }
  mcp_encode_type_Slot(&this->result, dest);
  mcp_encode_bef32(this->experience, dest);
  mcp_encode_varint(this->cook_time, dest);
}
void mcp_decode_type_Smelting(mcp_type_Smelting* this, mcp_buffer_t* src) {
  mcp_decode_string(&this->group, src);
  this->ingredient.size = mcp_decode_varint(src);
  this->ingredient.data = malloc(this->ingredient.size * sizeof(mcp_type_Slot));
  assertd_not_null("mcp_decode_type_Smelting", this->ingredient.data);
  for (size_t i = 0; i < this->ingredient.size; i++) {
    mcp_decode_type_Slot(&this->ingredient.data[i], src);
  }
  mcp_decode_type_Slot(&this->result, src);
  mcp_decode_bef32(&this->experience, src);
  this->cook_time = mcp_decode_varint(src);
}
void mcp_length_type_Smelting(mcp_type_Smelting* this, size_t* length) {
  mcp_length_string(this->group, length);
  length += mcp_length_varlong(this->ingredient.size);
  for (size_t i = 0; i < this->ingredient.size; i++) {
    mcp_length_type_Slot(&this->ingredient.data[i], length);
  }
  mcp_length_type_Slot(&this->result, length);
  *length += sizeof(this->experience);
  *length += mcp_length_varint(this->cook_time);
}

/**
 * minecraft tag
 */
void mcp_encode_type_Tag(mcp_type_Tag* this, mcp_buffer_t* dest) {
  mcp_encode_string(this->tag_name, dest);
  mcp_encode_varint(this->entries.size, dest);
  for (size_t i = 0; i < this->entries.size; i++) {
    mcp_encode_varint(this->entries.data[i], dest);
  }
}
void mcp_decode_type_Tag(mcp_type_Tag* this, mcp_buffer_t* src) { 
  mcp_decode_string(&this->tag_name, src);
  this->entries.size = mcp_decode_varint(src);
  this->entries.data = malloc(this->entries.size * sizeof(int32_t));
  assertd_not_null("mcp_decode_type_Smelting", this->entries.data);
  for (size_t i = 0; i < this->entries.size; i++) {
    this->entries.data[i] = mcp_decode_varint(src);
  }
}
void mcp_length_type_Tag(mcp_type_Tag* this, size_t* length) {
  mcp_length_string(this->tag_name, length);
  *length += mcp_length_varlong(this->entries.size);
  for (size_t i = 0; i < this->entries.size; i++) {
    *length += mcp_length_varint(this->entries.data[i]);
  }
}

/**
 * minecraft entity equipment
 * 
 * @todo decoder stub
 */
void mcp_encode_type_EntityEquipment(mcp_type_EntityEquipment* this, mcp_buffer_t* dest) {
  for (size_t i = 0; i < (this->equipments.size - 1); i++) {
    mcp_encode_byte(0x80 | this->equipments.data[i].slot, dest);
    mcp_encode_type_Slot(&this->equipments.data[i].item, dest);
  }
  mcp_encode_byte(this->equipments.data[this->equipments.size - 1].slot, dest);
  mcp_encode_type_Slot(&this->equipments.data[this->equipments.size - 1].item, dest);
}
void mcp_decode_type_EntityEquipment(mcp_type_EntityEquipment* this, mcp_buffer_t* src) { }
void mcp_length_type_EntityEquipment(mcp_type_EntityEquipment* this, size_t* length) {
  for (size_t i = 0; i < (this->equipments.size - 1); i++) {
    *length += sizeof(this->equipments.data[i].slot);
    mcp_length_type_Slot(&this->equipments.data[i].item, length);
  }
  *length += sizeof(this->equipments.data[this->equipments.size - 1].slot);
  mcp_length_type_Slot(&this->equipments.data[this->equipments.size - 1].item, length);
}

/**
 * minecraft entity metadata
 * 
 * @todo stub
 */
void mcp_encode_type_EntityMetadata(mcp_type_EntityMetadata* this, mcp_buffer_t* dest) { }
void mcp_decode_type_EntityMetadata(mcp_type_EntityMetadata* this, mcp_buffer_t* src) { }
void mcp_length_type_EntityMetadata(mcp_type_EntityMetadata* this, size_t* length) { }

/**
 * string
 * 
 * @warning strings are encoded length-prefixed and without null terminator, 
 *            but decoder returns regular null-terminated string
 */
void mcp_encode_string(char* this, mcp_buffer_t* dest) {
  size_t length = strlen(this);
  mcp_encode_varint(length, dest);
  memcpy(mcp_buffer_current(dest), this, length);
  mcp_buffer_increment(dest, length);
}
void mcp_decode_string(char** this, mcp_buffer_t* src) {
  size_t length = mcp_decode_varint(src);
  *this = malloc(length + 1);
  (*this)[length] = 0;
  memcpy(*this, mcp_buffer_current(src), length);
  mcp_buffer_increment(src, length);
}

/**
 * variable sized number
 */
void mcp_encode_varint(uint64_t src, mcp_buffer_t* dest) {
  for(; src >= 0x80; src >>= 7) {
    *mcp_buffer_current(dest) = (char) 0x80 | (src & 0x7F);
    mcp_buffer_increment(dest, 1);
  }
  *mcp_buffer_current(dest) = (char) src & 0x7F;
  mcp_buffer_increment(dest, 1);
}
uint64_t mcp_decode_varint(mcp_buffer_t* src) {
  int i = 0;
  char j;
  uint64_t dest = 0;
  do {
    j = *mcp_buffer_current(src);
    mcp_buffer_increment(src, 1);
    dest |= ((j & 0b01111111) << i);
    i += 7;
  } while (j & 0b10000000);
  return dest;
}

/**
 * variable sized number (from stream)
 */
void mcp_encode_stream_varint(uint64_t src, mcp_stream_t dest) {
  uint64_t tmp;
  for(; src >= 0x80; src >>= 7) {
    tmp = 0x80 | (src & 0x7F);
    mcp_stream_write(dest, (char*) &tmp, 1);
  }
  tmp = src & 0x7F;
  mcp_stream_write(dest, (char*) &tmp, 1);
}
uint64_t mcp_decode_stream_varint(mcp_stream_t src) {
  int i = 0;
  char j;
  uint64_t dest = 0;
  do {
    mcp_stream_read(src, &j, 1);
    dest |= ((j & 0b01111111) << i);
    i += 7;
  } while (j & 0b10000000);
  return dest;
}

/**
 * nbt stub
 */
void mcp_encode_type_NbtTagCompound(mcp_type_NbtTagCompound* this, mcp_buffer_t* dest) { }
void mcp_decode_type_NbtTagCompound(mcp_type_NbtTagCompound* this, mcp_buffer_t* src) { }
void mcp_read_type_NbtTagCompound(mcp_type_NbtTagCompound* this, mcp_buffer_t* src) { }