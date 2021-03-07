/**
 * @file codec.c
 * @author SpockBotMC
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief encoders/decoders for minecraft data types
 * @version 0.6
 * @date 2020-12-14
 */
      /* includes */
#include "mcp/codec.h" /* this */
#include <endian.h> /* byte swap */ 
#include <string.h> /* string operations */ 
#include <malloc.h> /* memory allocation */ 
#include "csafe/assertd.h" /* debug assertions */

      /* functions */
/**
 * @brief minecraft uuid
 */
inline void mcp_encode_type_UUID(mcp_type_UUID* this, mcp_buffer_t* dest) {
  mcp_encode_be64(&this->msb, dest);
  mcp_encode_be64(&this->lsb, dest);
}
inline void mcp_decode_type_UUID(mcp_type_UUID* this, mcp_buffer_t* src) {
  mcp_decode_be64(&this->msb, src);
  mcp_decode_be64(&this->lsb, src);
}

/**
 * @brief minecraft position
 */
inline void mcp_encode_type_Position(mcp_type_Position* this, mcp_buffer_t* dest) {
  uint64_t tmp = (
    ((uint64_t) this->x & 0x3FFFFFFUL) << 38 |
    ((uint64_t) this->z & 0x3FFFFFFUL) << 12 |
    ((uint64_t) this->y & 0xFFFUL)
  );
  mcp_encode_be64(&tmp, dest);
}
inline void mcp_decode_type_Position(mcp_type_Position* this, mcp_buffer_t* src) {
  uint64_t tmp;
  mcp_decode_be64(&tmp, src);
  if((this->x = tmp >> 38) & (1UL << 25))
    this->x -= 1UL << 26;
  if((this->z = tmp >> 12 & 0x3FFFFFFUL) & (1UL << 25))
    this->z -= 1UL << 26;
  if((this->y = tmp & 0xFFFUL) & (1UL << 11))
    this->y -= 1UL << 12;
}

/**
 * @brief minecraft container slot
 */
void mcp_encode_type_Slot(mcp_type_Slot* this, mcp_buffer_t* dest) {
  mcp_encode_byte(&this->present, dest);
  if (this->present) {
    mcp_encode_varint(this->item_id, dest);
    mcp_encode_byte((uint8_t*) &this->item_count, dest);
    if(this->nbt_data.has_value) {
      mcp_encode_type_NbtTagCompound(&this->nbt_data.value, dest);
    } else {
      uint8_t tag = MCP_NBT_TAG_END;
      mcp_encode_byte(&tag, dest);
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

/**
 * @brief minecraft particle
 */
void mcp_encode_type_Particle(mcp_type_Particle* this, mcp_buffer_t* dest) {
  switch(this->type) {
    case PARTICLE_BLOCK:
    case PARTICLE_FALLING_DUST:
      mcp_encode_varint(this->block_state, dest);
      break;

    case PARTICLE_DUST:
      mcp_encode_bef32(&this->red, dest);
      mcp_encode_bef32(&this->green, dest);
      mcp_encode_bef32(&this->blue, dest);
      mcp_encode_bef32(&this->scale, dest);
      break;

    case PARTICLE_ITEM:
      mcp_encode_type_Slot(&this->item, dest);
      break;
  }
}
void mcp_decode_type_Particle(mcp_type_Particle* this, mcp_type_ParticleType p_type, mcp_buffer_t* src) {
  this->type = p_type;
  switch (this->type) {
    case PARTICLE_BLOCK:
    case PARTICLE_FALLING_DUST:
      this->block_state = mcp_decode_varint(src);
      break;

    case PARTICLE_DUST:
      mcp_decode_be32((uint32_t*) &this->red, src);
      mcp_decode_be32((uint32_t*) &this->green, src);
      mcp_decode_be32((uint32_t*) &this->blue, src);
      mcp_decode_be32((uint32_t*) &this->scale, src);
      break;

    case PARTICLE_ITEM:
      mcp_decode_type_Slot(&this->item, src);
      break;
  }
}

/**
 * @brief minecraft item smelting
 */
void mcp_encode_type_Smelting(mcp_type_Smelting* this, mcp_buffer_t* dest) {
  mcp_encode_string(&this->group, dest);
  mcp_encode_varint(this->ingredient.size, dest);
  for (size_t i = 0; i < this->ingredient.size; i++) {
    mcp_encode_type_Slot(&this->ingredient.data[i], dest);
  }
  mcp_encode_type_Slot(&this->result, dest);
  mcp_encode_bef32(&this->experience, dest);
  mcp_encode_varint(this->cook_time, dest);
}
MALLOC void mcp_decode_type_Smelting(mcp_type_Smelting* this, mcp_buffer_t* src) {
  mcp_decode_string(&this->group, src);
  this->ingredient.size = mcp_decode_varint(src);
  this->ingredient.data = malloc(this->ingredient.size * sizeof(mcp_type_Slot));
  for (size_t i = 0; i < this->ingredient.size; i++) {
    mcp_decode_type_Slot(&this->ingredient.data[i], src);
  }
  mcp_decode_type_Slot(&this->result, src);
  mcp_decode_bef32(&this->experience, src);
  this->cook_time = mcp_decode_varint(src);
}

/**
 * @brief minecraft tag
 */
void mcp_encode_type_Tag(mcp_type_Tag* this, mcp_buffer_t* dest) {
  mcp_encode_string(&this->tag_name, dest);
  mcp_encode_varint(this->entries.size, dest);
  for (size_t i = 0; i < this->entries.size; i++) {
    mcp_encode_varint(this->entries.data[i], dest);
  }
}
MALLOC void mcp_decode_type_Tag(mcp_type_Tag* this, mcp_buffer_t* src) { //todo deal with memory leaks (in handlers? generate code for each packet to dealloc?)
//todo FREE function for each MALLOC function
  mcp_decode_string(&this->tag_name, src);
  this->entries.size = mcp_decode_varint(src);
  this->entries.data = malloc(this->entries.size * sizeof(int32_t));
  for (size_t i = 0; i < this->entries.size; i++) {
    this->entries.data[i] = mcp_decode_varint(src);
  }
}

/**
 * @brief minecraft entity equipment
 */
void mcp_encode_type_EntityEquipment(mcp_type_EntityEquipment* this, mcp_buffer_t* dest) {
  for (size_t i = 0; i < (this->equipments.size - 1); i++) {
    uint8_t tmp = (0x80 | this->equipments.data[i].slot);
    mcp_encode_byte(&tmp, dest);
    mcp_encode_type_Slot(&this->equipments.data[i].item, dest);
  }
  mcp_encode_byte((uint8_t*) &this->equipments.data[this->equipments.size - 1].slot, dest);
  mcp_encode_type_Slot(&this->equipments.data[this->equipments.size - 1].item, dest);
}
void mcp_decode_type_EntityEquipment(mcp_type_EntityEquipment* this, mcp_buffer_t* src) {
  //todo VLA
  this->equipments.size = 0;
  this->equipments.data = NULL;
  /*this->equipments.size = 0;
  uint8_t slot;
  do {
    slot = dec_byte(src);
    this->equipments.emplace_back();
    this->equipments.back().slot = slot & 0x7F;
    this->equipments.back().item.decode(src);
  } while(slot & 0x80);*/
}

/**
 * @brief minecraft entity metadata
 */
void mcp_encode_type_EntityMetadata(mcp_type_EntityMetadata* this, mcp_buffer_t* dest) {
  //todo nbt

  /*for(auto &el : data) {
    enc_byte(dest, el.index);
    enc_varint(dest, el.type);
    switch(el.type) {
      case METATAG_BYTE:
      case METATAG_BOOLEAN:
        enc_byte(dest, (int8_t) el.value);
        break;
      case METATAG_VARINT:
      case METATAG_DIRECTION:
      case METATAG_BLOCKID:
      case METATAG_POSE:
        enc_varint(dest, (int32_t) el.value);
        break;
      case METATAG_FLOAT:
        enc_bef32(dest, (float) el.value);
        break;
      case METATAG_STRING:
      case METATAG_CHAT:
        enc_string(dest, (const char*) el.value);
        break;
      case METATAG_OPTCHAT: {
        auto str = (const char*) el.value;
        enc_byte(dest, str.has_value());
        if(str.has_value())
          enc_string(dest, str.value());
      }
        break;
      case METATAG_SLOT:
        (mc_slot) el.value.encode(dest);
        break;
      case METATAG_ROTATION:
        for(auto& el : (std::array<float, 3>) el.value)
          enc_bef32(dest, el);
      case METATAG_POSITION:
        enc_position(dest, (mc_position) el.value);
        break;
      case METATAG_OPTPOSITION: {
        auto pos = (mc_position) el.value;
        enc_byte(dest, pos.has_value());
        if(pos.has_value())
          enc_position(dest, pos.value());
      }
        break;
      case METATAG_OPTUUID: {
        auto uuid = (mc_uuid) el.value;
        enc_byte(dest, uuid.has_value());
        if(uuid.has_value())
          enc_uuid(dest, uuid.value());
      }
        break;
      case METATAG_NBT:
        (nbt::TagCompound) el.value.encode_full(dest);
        break;
      case METATAG_PARTICLE:
        enc_varint(dest, (MCParticle) el.value.type);
        (MCParticle) el.value.encode(dest);
        break;
      case METATAG_VILLAGERDATA:
        for(auto& el : (std::array<int32_t, 3>) el.value)
          enc_varint(dest, el);
        break;
      case METATAG_OPTVARINT: {
        auto varint = (int32_t) el.value;
        enc_byte(dest, varint.has_value());
        if(varint.has_value())
          enc_varint(dest, varint.value());
      }
        break;
    }
  }
  enc_byte(dest, 0xFF);*/
}
void mcp_decode_type_EntityMetadata(mcp_type_EntityMetadata* this, mcp_buffer_t* src) {
  this->data = NULL;
  /*data.clear();
  uint8_t index = dec_byte(src);
  while(index != 0xFF) {
    auto& tag = data.emplace_back();
    tag.index = index;
    tag.type = dec_varint(src);
    switch(tag.type) {
      case METATAG_BYTE:
      case METATAG_BOOLEAN:
        tag.value = dec_byte(src);
        break;
      case METATAG_VARINT:
      case METATAG_DIRECTION:
      case METATAG_BLOCKID:
      case METATAG_POSE:
        // Not sure why this is considered ambiguous, maybe conflict with int8?
        tag.value.emplace<int32_t>(dec_varint(src));
        break;
      case METATAG_FLOAT:
        tag.value = dec_bef32(src);
        break;
      case METATAG_STRING:
      case METATAG_CHAT:
        tag.value = dec_string(src);
        break;
      case METATAG_OPTCHAT: {
        auto& str = tag.value.emplace<const char*>();
        if(dec_byte(src))
          str = dec_string(src);
      }
        break;
      case METATAG_SLOT: {
        auto& slot = tag.value.emplace<mc_slot>();
        slot.decode(src);
      }
        break;
      case METATAG_ROTATION: {
        auto& rot = tag.value.emplace<std::array<float, 3>>();
        for(auto &el : rot)
          el = dec_bef32(src);
      }
        break;
      case METATAG_POSITION:
        tag.value = dec_position(src);
        break;
      case METATAG_OPTPOSITION: {
        auto& pos = tag.value.emplace<mc_position>();
        if(dec_byte(src))
          pos = dec_position(src);
      }
        break;
      case METATAG_OPTUUID: {
        auto& uuid = tag.value.emplace<mc_uuid>();
        if(dec_byte(src))
          uuid = dec_uuid(src);
      }
        break;
      case METATAG_NBT: {
        auto& nbt_tag = tag.value.emplace<nbt::TagCompound>();
        nbt_tag.decode_full(src);
      }
        break;
      case METATAG_PARTICLE: {
        auto& particle = tag.value.emplace<MCParticle>();
        particle.decode(src, (particle_type) dec_varint(src));
      }
        break;
      case METATAG_VILLAGERDATA: {
        auto& data = tag.value.emplace<std::array<int32_t, 3>>();
        for(auto &el : data)
          el = dec_varint(src);
      }
        break;
      case METATAG_OPTVARINT: {
        auto& varint = tag.value.emplace<int32_t>();
        if(dec_byte(src))
          varint = dec_varint(src);
      }
        break;
    }
    index = dec_byte(src);
  }*/
}

/**
 * @brief byte
 */
inline void mcp_encode_byte(uint8_t* this, mcp_buffer_t* dest) {
  mcp_buffer_write(dest, (char*) this, SINGLE_BYTE);
}
inline void mcp_decode_byte(uint8_t* this, mcp_buffer_t* src) {
  mcp_buffer_read(src, (char*) this, SINGLE_BYTE);
}

/**
 * @brief big endian uint16
 */
inline void mcp_encode_be16(uint16_t* this, mcp_buffer_t* dest) {
  uint16_t tmp = htobe16(*this);
  mcp_buffer_write_variable(dest, tmp);
}
inline void mcp_decode_be16(uint16_t* this, mcp_buffer_t* src) {
  uint16_t tmp;
  mcp_buffer_read_variable(src, tmp);
  *this = be16toh(tmp);
}

/**
 * @brief little endian uint16
 */
inline void mcp_encode_le16(uint16_t* this, mcp_buffer_t* dest) {
  uint16_t tmp = htole16(*this);
  mcp_buffer_write_variable(dest, tmp);
}
inline void mcp_decode_le16(uint16_t* this, mcp_buffer_t* src) {
  uint16_t tmp;
  mcp_buffer_read_variable(src, tmp);
  *this = le16toh(tmp);
}

/**
 * @brief big endian uint32
 */
inline void mcp_encode_be32(uint32_t* this, mcp_buffer_t* dest) {
  uint32_t tmp = htobe32(*this);
  mcp_buffer_write_variable(dest, tmp);
}
inline void mcp_decode_be32(uint32_t* this, mcp_buffer_t* src) {
  uint32_t tmp;
  mcp_buffer_read_variable(src, tmp);
  *this = be32toh(tmp);
}

/**
 * @brief little endian uint32
 */
inline void mcp_encode_le32(uint32_t* this, mcp_buffer_t* dest) {
  uint32_t tmp = htole32(*this);
  mcp_buffer_write_variable(dest, tmp);
}
inline void mcp_decode_le32(uint32_t* this, mcp_buffer_t* src) {
  uint32_t tmp;
  mcp_buffer_read_variable(src, tmp);
  *this = le32toh(tmp);
}

/**
 * @brief big endian uint64
 */
inline void mcp_encode_be64(uint64_t* this, mcp_buffer_t* dest) {
  uint64_t tmp = htobe64(*this);
  mcp_buffer_write_variable(dest, tmp);
}
inline void mcp_decode_be64(uint64_t* this, mcp_buffer_t* src) {
  uint64_t tmp;
  mcp_buffer_read_variable(src, tmp);
  *this = be64toh(tmp);
}

/**
 * @brief little endian uint64
 */
inline void mcp_encode_le64(uint64_t* this, mcp_buffer_t* dest) {
  uint64_t tmp = htole64(*this);
  mcp_buffer_write_variable(dest, tmp);
}
inline void mcp_decode_le64(uint64_t* this, mcp_buffer_t* src) {
  uint64_t tmp;
  mcp_buffer_read_variable(src, tmp);
  *this = le64toh(tmp);
}

/**
 * @brief big endian float32
 */
inline void mcp_encode_bef32(float* this, mcp_buffer_t* dest) {
  uint32_t tmp = htobe32((uint32_t) *this);
  mcp_buffer_write_variable(dest, tmp);
}
inline void mcp_decode_bef32(float* this, mcp_buffer_t* src) {
  uint32_t tmp;
  mcp_buffer_read_variable(src, tmp);
  *this = (float) be32toh(tmp);
}

/**
 * @brief little endian float32
 */
inline void mcp_encode_lef32(float* this, mcp_buffer_t* dest) {
  uint32_t tmp = htole32((uint32_t) *this);
  mcp_buffer_write_variable(dest, tmp);
}

inline void mcp_decode_lef32(float* this, mcp_buffer_t* src) {
  uint32_t tmp;
  mcp_buffer_read_variable(src, tmp);
  *this = (float) le32toh(tmp);
}

/**
 * @brief big endian float64
 */
inline void mcp_encode_bef64(double* this, mcp_buffer_t* dest) {
  uint64_t tmp = htobe64((uint64_t) *this);
  mcp_buffer_write_variable(dest, tmp);
}
inline void mcp_decode_bef64(double* this, mcp_buffer_t* src) {
  uint64_t tmp;
  mcp_buffer_read_variable(src, tmp);
  *this = (double) be64toh(tmp);
}

/**
 * @brief little endian float64
 */
inline void mcp_encode_lef64(double* this, mcp_buffer_t* dest) {
  uint64_t tmp = htole64((uint64_t) *this);
  mcp_buffer_write_variable(dest, tmp);
}
inline void mcp_decode_lef64(double* this, mcp_buffer_t* src) {
  uint64_t tmp;
  mcp_buffer_read_variable(src, tmp);
  *this = (double) le64toh(tmp);
}

/**
 * @brief string
 */
void mcp_encode_string(char** this, mcp_buffer_t* dest) {
  size_t length = strlen(*this);
  mcp_encode_varint(length, dest);
  mcp_buffer_write(dest, *this, length);
}
MALLOC void mcp_decode_string(char** this, mcp_buffer_t* src) {
  size_t length = mcp_decode_varint(src);
  char* string = malloc(length + 1);
  mcp_buffer_read(src, string, length);
  string[length] = 0;
  *this = string;
}
inline size_t mcp_length_string(const char* src) {
  size_t length = strlen(src);
  return mcp_length_varlong(length) + length;
}

/**
 * @brief buffer
 */
inline void mcp_encode_buffer(char_vector_t* this, mcp_buffer_t* dest) {
  mcp_buffer_write(dest, this->data, this->size);
}
inline void mcp_decode_buffer(char_vector_t* this, size_t length, mcp_buffer_t* src) {
  this->size = length;
  this->data = malloc(sizeof(char) * length);
  mcp_buffer_read(src, this->data, this->size);
}

/**
 * @brief variable sized number
 */
void mcp_encode_varint(uint64_t src, mcp_buffer_t* dest) {
  uint64_t tmp;
  for(; src >= 0x80; src >>= 7) {
    tmp = 0x80 | (src & 0x7F);
    mcp_buffer_write(dest, (char*) &tmp, SINGLE_BYTE);
  }
  tmp = src & 0x7F;
  mcp_buffer_write(dest, (char*) &tmp, SINGLE_BYTE);
}
uint64_t mcp_decode_varint(mcp_buffer_t* src) {
  int i = 0;
  char j;
  uint64_t dest = 0;
  do {
    mcp_buffer_read(src, &j, SINGLE_BYTE);
    dest |= ((j & 0b01111111) << i);
    i += 7;
  } while (j & 0b10000000);
  return dest;
}
/**
 * @brief calculate varnum size for fixed integer
 */
inline size_t mcp_length_varint(uint32_t varint) {
  if(varint < (1 << 7))
    return 1;
  if(varint < (1 << 14))
    return 2;
  if(varint < (1 << 21))
    return 3;
  if(varint < (1 << 28))
    return 4;
  return 5;
}
inline size_t mcp_length_varlong(uint64_t varlong) {
  if(varlong < (1 << 7))
    return 1;
  if(varlong < (1 << 14))
    return 2;
  if(varlong < (1 << 21))
    return 3;
  if(varlong < (1 << 28))
    return 4;
  if(varlong < (1ULL << 35))
    return 5;
  if(varlong < (1ULL << 42))
    return 6;
  if(varlong < (1ULL << 49))
    return 7;
  if(varlong < (1ULL << 56))
    return 8;
  if(varlong < (1ULL << 63))
    return 9;
  return 10;
}

/**
 * @brief variable sized number (from stream)
 */
void mcp_encode_stream_varint(uint64_t src, mcp_stream_t dest) {
  uint64_t tmp;
  for(; src >= 0x80; src >>= 7) {
    tmp = 0x80 | (src & 0x7F);
    mcp_stream_write(dest, (char*) &tmp, SINGLE_BYTE);
  }
  tmp = src & 0x7F;
  mcp_stream_write(dest, (char*) &tmp, SINGLE_BYTE);
}
uint64_t mcp_decode_stream_varint(mcp_stream_t src) {
  int i = 0;
  char j;
  uint64_t dest = 0;
  do {
    mcp_stream_read(src, &j, SINGLE_BYTE);
    dest |= ((j & 0b01111111) << i);
    i += 7;
  } while (j & 0b10000000);
  return dest;
}

/**
 * @brief nbt stub
 */
void mcp_encode_type_NbtTagCompound(mcp_type_NbtTagCompound* this, mcp_buffer_t* dest) { }
void mcp_decode_type_NbtTagCompound(mcp_type_NbtTagCompound* this, mcp_buffer_t* src) { }
void mcp_read_type_NbtTagCompound(mcp_type_NbtTagCompound* this, mcp_buffer_t* src) { }