/**
 * @file codec.c
 * @author SpockBotMC
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief encoders/decoders for minecraft data types
 * @version 0.5
 * @date 2020-12-14
 */
      /* includes */
#include "mcp/codec.h" /* this */
#include <endian.h> /* byte swap */ 
#include <string.h> /* string operations */ 
#include <malloc.h> /* memory allocation */ 
#include "mcp/varnum.h" /* varnum size */

      /* functions */
/**
 * @brief minecraft uuid
 */
void mcp_type_UUID_encode(mcp_type_UUID* this, stream_t dest) {
  mcp_be64_encode(&this->msb, dest);
  mcp_be64_encode(&this->lsb, dest);
}
void mcp_type_UUID_decode(mcp_type_UUID* this, stream_t src) {
  mcp_be64_decode(&this->msb, src);
  mcp_be64_decode(&this->lsb, src);
}

/**
 * @brief minecraft position
 */
void mcp_type_Position_encode(mcp_type_Position* this, stream_t dest) {
  uint64_t tmp = (
    ((uint64_t) this->x & 0x3FFFFFFUL) << 38 |
    ((uint64_t) this->z & 0x3FFFFFFUL) << 12 |
    ((uint64_t) this->y & 0xFFFUL)
  );
  mcp_be64_encode(&tmp, dest);
}
void mcp_type_Position_decode(mcp_type_Position* this, stream_t src) {
  uint64_t tmp;
  mcp_be64_decode(&tmp, src);
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
void mcp_type_Slot_encode(mcp_type_Slot* this, stream_t dest) {
  mcp_byte_encode(&this->present, dest);
  if (this->present) {
    mcp_varint_encode((uint64_t*) &this->item_id, dest);
    mcp_byte_encode((uint8_t*) &this->item_count, dest);
    if(this->nbt_data.has_value) {
      mcp_type_NbtTagCompound_encode(&this->nbt_data.value, dest);
    } else {
      uint8_t tag = MCP_NBT_TAG_END;
      mcp_byte_encode(&tag, dest);
    }
  }
}
void mcp_type_Slot_decode(mcp_type_Slot* this, stream_t src) {
  mcp_byte_decode(&this->present, src);
  if (this->present) {
    mcp_varint_decode((uint64_t*) &this->item_id, src);
    mcp_byte_decode((uint8_t*) &this->item_count, src);
    uint8_t tag;
    mcp_byte_decode(&tag, src);
    if (tag == MCP_NBT_TAG_COMPOUND) {
      this->nbt_data.has_value = true;
      mcp_type_NbtTagCompound_read(&this->nbt_data.value, src);
    }
  }
}

/**
 * @brief minecraft particle
 */
void mcp_type_Particle_encode(mcp_type_Particle* this, stream_t dest) {
  switch(this->type) {
    case PARTICLE_BLOCK:
    case PARTICLE_FALLING_DUST:
      mcp_varint_encode((uint64_t*) &this->block_state, dest);
      break;

    case PARTICLE_DUST:
      mcp_be32_encode((uint32_t*) &this->red, dest);
      mcp_be32_encode((uint32_t*) &this->green, dest);
      mcp_be32_encode((uint32_t*) &this->blue, dest);
      mcp_be32_encode((uint32_t*) &this->scale, dest);
      break;

    case PARTICLE_ITEM:
      mcp_type_Slot_encode(&this->item, dest);
      break;
  }
}
void mcp_type_Particle_decode(mcp_type_Particle* this, mcp_type_ParticleType p_type, stream_t src) {
  this->type = p_type;
  switch (this->type) {
    case PARTICLE_BLOCK:
    case PARTICLE_FALLING_DUST:
      mcp_varint_decode((uint64_t*) &this->block_state, src);
      break;

    case PARTICLE_DUST:
      mcp_be32_decode((uint32_t*) &this->red, src);
      mcp_be32_decode((uint32_t*) &this->green, src);
      mcp_be32_decode((uint32_t*) &this->blue, src);
      mcp_be32_decode((uint32_t*) &this->scale, src);
      break;

    case PARTICLE_ITEM:
      mcp_type_Slot_decode(&this->item, src);
      break;
  }
}

/**
 * @brief minecraft item smelting
 */
void mcp_type_Smelting_encode(mcp_type_Smelting* this, stream_t dest) {
  mcp_string_encode(&this->group, dest);
  mcp_varint_encode(&this->ingredient.size, dest);
  for (size_t i = 0; i < this->ingredient.size; i++) {
    mcp_type_Slot_encode(&this->ingredient.data[i], dest);
  }
  mcp_type_Slot_encode(&this->result, dest);
  mcp_bef32_encode(&this->experience, dest);
  mcp_varint_encode((uint64_t*) &this->cook_time, dest);
}
MALLOC void mcp_type_Smelting_decode(mcp_type_Smelting* this, stream_t src) {
  mcp_string_decode(&this->group, src);
  mcp_varint_decode(&this->ingredient.size, src);
  this->ingredient.data = malloc(this->ingredient.size * sizeof(mcp_type_Slot));
  for (size_t i = 0; i < this->ingredient.size; i++) {
    mcp_type_Slot_decode(&this->ingredient.data[i], src);
  }
  mcp_type_Slot_decode(&this->result, src);
  mcp_bef32_decode(&this->experience, src);
  mcp_varint_decode((uint64_t*) &this->cook_time, src);
}

/**
 * @brief minecraft tag
 */
void mcp_type_Tag_encode(mcp_type_Tag* this, stream_t dest) {
  mcp_string_encode(&this->tag_name, dest);
  mcp_varint_encode(&this->entries.size, dest);
  for (size_t i = 0; i < this->entries.size; i++) {
    mcp_varint_encode((uint64_t*) &this->entries.data[i], dest);
  }
}
MALLOC void mcp_type_Tag_decode(mcp_type_Tag* this, stream_t src) { //todo deal with memory leaks (in handlers? generate code for each packet to dealloc?)
//todo FREE function for each MALLOC function
  mcp_string_decode(&this->tag_name, src);
  mcp_varint_decode(&this->entries.size, src);
  this->entries.data = malloc(this->entries.size * sizeof(int32_t));
  for (size_t i = 0; i < this->entries.size; i++) {
    mcp_varint_decode((uint64_t*) &this->entries.data[i], src);
  }
}

/**
 * @brief minecraft entity equipment
 */
void mcp_type_EntityEquipment_encode(mcp_type_EntityEquipment* this, stream_t dest) {
  for (size_t i = 0; i < (this->equipments.size - 1); i++) {
    uint8_t tmp = (0x80 | this->equipments.data[i].slot);
    mcp_byte_encode(&tmp, dest);
    mcp_type_Slot_encode(&this->equipments.data[i].item, dest);
  }
  mcp_byte_encode((uint8_t*) &this->equipments.data[this->equipments.size - 1].slot, dest);
  mcp_type_Slot_encode(&this->equipments.data[this->equipments.size - 1].item, dest);
}
void mcp_type_EntityEquipment_decode(mcp_type_EntityEquipment* this, stream_t src) {
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
void mcp_type_EntityMetadata_encode(mcp_type_EntityMetadata* this, stream_t dest) {
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
void mcp_type_EntityMetadata_decode(mcp_type_EntityMetadata* this, stream_t src) {
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
void mcp_byte_encode(uint8_t* this, stream_t dest) {
  stream_write(dest, this, SINGLE_BYTE);
}
void mcp_byte_decode(uint8_t* this, stream_t src) {
  stream_read(src, this, SINGLE_BYTE);
}

/**
 * @brief big endian uint16
 */
void mcp_be16_encode(uint16_t* this, stream_t dest) {
  uint16_t tmp = htobe16(*this);
  stream_write_variable(dest, tmp);
}
void mcp_be16_decode(uint16_t* this, stream_t src) {
  uint16_t tmp;
  stream_read_variable(src, tmp);
  *this = be16toh(tmp);
}

/**
 * @brief little endian uint16
 */
void mcp_le16_encode(uint16_t* this, stream_t dest) {
  uint16_t tmp = htole16(*this);
  stream_write_variable(dest, tmp);
}
void mcp_le16_decode(uint16_t* this, stream_t src) {
  uint16_t tmp;
  stream_read_variable(src, tmp);
  *this = le16toh(tmp);
}

/**
 * @brief big endian uint32
 */
void mcp_be32_encode(uint32_t* this, stream_t dest) {
  uint32_t tmp = htobe32(*this);
  stream_write_variable(dest, tmp);
}
void mcp_be32_decode(uint32_t* this, stream_t src) {
  uint32_t tmp;
  stream_read_variable(src, tmp);
  *this = be32toh(tmp);
}

/**
 * @brief little endian uint32
 */
void mcp_le32_encode(uint32_t* this, stream_t dest) {
  uint32_t tmp = htole32(*this);
  stream_write_variable(dest, tmp);
}
void mcp_le32_decode(uint32_t* this, stream_t src) {
  uint32_t tmp;
  stream_read_variable(src, tmp);
  *this = le32toh(tmp);
}

/**
 * @brief big endian uint64
 */
void mcp_be64_encode(uint64_t* this, stream_t dest) {
  uint64_t tmp = htobe64(*this);
  stream_write_variable(dest, tmp);
}
void mcp_be64_decode(uint64_t* this, stream_t src) {
  uint64_t tmp;
  stream_read_variable(src, tmp);
  *this = be64toh(tmp);
}

/**
 * @brief little endian uint64
 */
void mcp_le64_encode(uint64_t* this, stream_t dest) {
  uint64_t tmp = htole64(*this);
  stream_write_variable(dest, tmp);
}
void mcp_le64_decode(uint64_t* this, stream_t src) {
  uint64_t tmp;
  stream_read_variable(src, tmp);
  *this = le64toh(tmp);
}

/**
 * @brief big endian float32
 */
void mcp_bef32_encode(float* this, stream_t dest) {
  uint32_t tmp = htobe32((uint32_t) *this);
  stream_write_variable(dest, tmp);
}
void mcp_bef32_decode(float* this, stream_t src) {
  uint32_t tmp;
  stream_read_variable(src, tmp);
  *this = (float) be32toh(tmp);
}

/**
 * @brief little endian float32
 */
void mcp_lef32_encode(float* this, stream_t dest) {
  uint32_t tmp = htole32((uint32_t) *this);
  stream_write_variable(dest, tmp);
}

void mcp_lef32_decode(float* this, stream_t src) {
  uint32_t tmp;
  stream_read_variable(src, tmp);
  *this = (float) le32toh(tmp);
}

/**
 * @brief big endian float64
 */
void mcp_bef64_encode(double* this, stream_t dest) {
  uint64_t tmp = htobe64((uint64_t) *this);
  stream_write_variable(dest, tmp);
}
void mcp_bef64_decode(double* this, stream_t src) {
  uint64_t tmp;
  stream_read_variable(src, tmp);
  *this = (double) be64toh(tmp);
}

/**
 * @brief little endian float64
 */
void mcp_lef64_encode(double* this, stream_t dest) {
  uint64_t tmp = htole64((uint64_t) *this);
  stream_write_variable(dest, tmp);
}
void mcp_lef64_decode(double* this, stream_t src) {
  uint64_t tmp;
  stream_read_variable(src, tmp);
  *this = (double) le64toh(tmp);
}

/**
 * @brief string
 */
void mcp_string_encode(char** this, stream_t dest) {
  size_t length = strlen(*this);
  mcp_varint_encode((uint64_t*) &length, dest);
  stream_write(dest, *this, length);
}
MALLOC void mcp_string_decode(char** this, stream_t src) {
  size_t length;
  mcp_varint_decode((uint64_t*) &length, src);
  char* string = malloc(length + 1);
  stream_read(src, string, length);
  string[length] = 0;
  *this = string;
}
size_t size_string(const char* src) {
  size_t length = strlen(src);
  return size_varlong(length) + length;
}

/**
 * @brief buffer
 */
void mcp_buffer_encode(char_vector_t* this, stream_t dest) {
  stream_write(dest, this->data, this->size);
}
void mcp_buffer_decode(char_vector_t* this, size_t length, stream_t src) {
  this->size = length;
  this->data = malloc(sizeof(char) * length);
  stream_read(src, this->data, this->size);
}

/**
 * @brief variable sized number
 */
void mcp_varint_encode(uint64_t* this, stream_t dest) {
  uint8_t tmp;
  uint64_t src = *this;
  for(; src >= 0x80; src >>= 7) {
    tmp = 0x80 | (src & 0x7F);
    stream_write_variable(dest, tmp);
  }
  tmp = src & 0x7F;
  stream_write_variable(dest, tmp);
}
void mcp_varint_decode(uint64_t* this, stream_t src) {
  int i = 0;
  uint64_t j = 0;
  *this = 0;
  stream_read(src, &j, SINGLE_BYTE);
  for(; j & 0x80; i += 7) {
    stream_read(src, &j, SINGLE_BYTE);
    *this |= (j & 0x7F) << i;
  }
  *this |= j << i;
}