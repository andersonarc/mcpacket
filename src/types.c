/**
 * @file types.c
 * @author SpockBotMC
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief data structures and encoders/decoders for minecraft data types
 * @version 0.4
 * @date 2020-12-14
 */
      /* includes */
#include "mcp/types.h" /* this */

      /* minecraft data encoders and decoders */
/**
 * @brief minecraft UUID
 */
void enc_uuid(stream_t dest, mc_uuid src) {
  enc_be64(dest, src.msb);
  enc_be64(dest, src.lsb);
}
mc_uuid dec_uuid(stream_t src) {
  mc_uuid dest;
  dest.msb = dec_be64(src);
  dest.lsb = dec_be64(src);
  return dest;
}

/**
 * @brief minecraft position
 */
void enc_position(stream_t dest, mc_position src) {
  uint64_t tmp = (
    ((uint64_t) src.x & 0x3FFFFFFUL) << 38 |
    ((uint64_t) src.z & 0x3FFFFFFUL) << 12 |
    ((uint64_t) src.y & 0xFFFUL)
  );
  enc_be64(dest, tmp);
}
mc_position dec_position(stream_t src) {
  mc_position dest;
  uint64_t tmp = dec_be64(src);
  if((dest.x = tmp >> 38) & (1UL << 25))
    dest.x -= 1UL << 26;
  if((dest.z = tmp >> 12 & 0x3FFFFFFUL) & (1UL << 25))
    dest.z -= 1UL << 26;
  if((dest.y = tmp & 0xFFFUL) & (1UL << 11))
    dest.y -= 1UL << 12;
  return dest;
}

/**
 * @brief minecraft container slot
 */
void mc_slot_encode(stream_t dest, mc_slot* this) {
  enc_byte(dest, this->present);
  if (this->present) {
    enc_varint(dest, (uint64_t) this->item_id);
    enc_byte(dest, (uint8_t) this->item_count);
    if(this->nbt_data.has_value) {
      nbt_encode_full(dest, this->nbt_data.value);
    } else {
      enc_byte(dest, NBT_TAG_END);
    }
  }
}
void mc_slot_decode(stream_t src, mc_slot* this) {
  this->present = dec_byte(src);
  if (this->present) {
    this->item_id = (int32_t) dec_varint(src);
    this->item_count = (int8_t) dec_byte(src);
    if (dec_byte(src) == NBT_TAG_COMPOUND) {
      this->nbt_data.has_value = true;
      this->nbt_data.value = nbt_read_string(src);
    }
  }
}

/**
 * @brief minecraft particle
 */
void mc_particle_encode(stream_t dest, mc_particle* this) {
  switch(this->type) {
    case PARTICLE_BLOCK:
    case PARTICLE_FALLING_DUST:
      enc_varint(dest, (uint64_t) this->block_state);
      break;

    case PARTICLE_DUST:
      enc_be32(dest, (uint32_t) this->red);
      enc_be32(dest, (uint32_t) this->green);
      enc_be32(dest, (uint32_t) this->blue);
      enc_be32(dest, (uint32_t) this->scale);
      break;

    case PARTICLE_ITEM:
      mc_slot_encode(dest, &this->item);
      break;
  }
}
void mc_particle_decode(stream_t src, mc_particle* this, mc_particle_type p_type) {
  this->type = p_type;
  switch (this->type) {
    case PARTICLE_BLOCK:
    case PARTICLE_FALLING_DUST:
      this->block_state = (int32_t) dec_varint(src);
      break;

    case PARTICLE_DUST:
      this->red = (float) dec_be32(src);
      this->green = (float) dec_be32(src);
      this->blue = (float) dec_be32(src);
      this->scale = (float) dec_be32(src);
      break;

    case PARTICLE_ITEM:
      mc_slot_decode(src, &this->item);
      break;
  }
}

/**
 * @brief minecraft item smelting
 */
void mc_smelting_encode(stream_t dest, mc_smelting* this) {
  enc_string(dest, this->group);
  enc_varint(dest, this->ingredient.size);
  for (size_t i = 0; i < this->ingredient.size; i++) {
    mc_slot_encode(dest, &this->ingredient.data[i]);
  }
  mc_slot_encode(dest, &this->result);
  enc_bef32(dest, this->experience);
  enc_varint(dest, (uint64_t) this->cook_time);
}
MALLOC void mc_smelting_decode(stream_t src, mc_smelting* this) {
  this->group = dec_string(src);
  this->ingredient.size = (size_t) dec_varint(src);
  this->ingredient.data = malloc(this->ingredient.size * sizeof(mc_slot));
  for (size_t i = 0; i < this->ingredient.size; i++) {
    mc_slot_decode(src, &this->ingredient.data[i]);
  }
  mc_slot_decode(src, &this->result);
  this->experience = dec_bef32(src);
  this->cook_time = (int32_t) dec_varint(src);
}

/**
 * @brief minecraft tag
 */
void mc_tag_encode(stream_t dest, mc_tag* this) {
  enc_string(dest, this->tag_name);
  enc_varint(dest, this->entries.size);
  for (size_t i = 0; i < this->entries.size; i++) {
    enc_varint(dest, (uint64_t) this->entries.data[i]);
  }
}
void mc_tag_decode(stream_t src, mc_tag* this) {
  this->tag_name = dec_string(src);
  this->entries.size = (size_t) dec_varint(src);
  this->entries.data = malloc(this->entries.size * sizeof(int32_t));
  for (size_t i = 0; i < this->entries.size; i++) {
    this->entries.data[i] = (int32_t) dec_varint(src);
  }
}

/**
 * @brief minecraft entity equipment
 */
void mc_entity_equipment_encode(stream_t dest, mc_entity_equipment* this) {
  for (size_t i = 0; i < (this->equipments.size - 1); i++) {
    enc_byte(dest, (uint8_t) (0x80 | this->equipments.data[i].slot));
    mc_slot_encode(dest, &this->equipments.data[i].item);
  }
  mc_item last = this->equipments.data[this->equipments.size - 1];
  enc_byte(dest, (uint8_t) last.slot);
  mc_slot_encode(dest, &last.item);
}
void mc_entity_equipment_decode(stream_t src, mc_entity_equipment* this) {
  //todo VLA

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
void mc_entity_metadata_encode(stream_t dest, mc_entity_metadata* this) {
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
void mc_entity_metadata_decode(stream_t src, mc_entity_metadata* this) {
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

      /* primitive type encoders and decoders */
/**
 * @brief uint8
 */
void enc_byte(stream_t dest, const uint8_t src) {
  write(dest, &src, sizeof(uint8_t));
}
uint8_t dec_byte(stream_t src) {
  uint8_t dest;
  read(src, &dest, sizeof(uint8_t));
  return dest;
}

/**
 * @brief big endian uint16
 */
void enc_be16(stream_t dest, uint16_t src) {
  src = htobe16(src);
  write(dest, &src, sizeof(uint16_t));
}
uint16_t dec_be16(stream_t src) {
  uint16_t dest;
  read(src, &dest, sizeof(uint16_t));
  return be16toh(dest);
}

/**
 * @brief little endian uint16
 */
void enc_le16(stream_t dest, uint16_t src) {
  src = htole16(src);
  write(dest, &src, sizeof(uint16_t));
}
uint16_t dec_le16(stream_t src) {
  uint16_t dest;
  read(src, &dest, sizeof(uint16_t));
  return le16toh(dest);
}

/**
 * @brief big endian uint32
 */
void enc_be32(stream_t dest, uint32_t src) {
  src = htobe32(src);
  write(dest, &src, sizeof(uint32_t));
}
uint32_t dec_be32(stream_t src) {
  uint32_t dest;
  read(src, &dest, sizeof(uint32_t));
  return be32toh(dest);
}

/**
 * @brief little endian uint32
 */
void enc_le32(stream_t dest, uint32_t src) {
  src = htole32(src);
  write(dest, &src, sizeof(uint32_t));
}
uint32_t dec_le32(stream_t src) {
  uint32_t dest;
  read(src, &dest, sizeof(uint32_t));
  return le32toh(dest);
}

/**
 * @brief big endian uint64
 */
void enc_be64(stream_t dest, uint64_t src) {
  src = htobe64(src);
  write(dest, &src, sizeof(uint64_t));
}
uint64_t dec_be64(stream_t src) {
  uint64_t dest;
  read(src, &dest, sizeof(uint64_t));
  return be64toh(dest);
}

/**
 * @brief little endian uint64
 */
void enc_le64(stream_t dest, uint64_t src) {
  src = htole64(src);
  write(dest, &src, sizeof(uint64_t));
}
uint64_t dec_le64(stream_t src) {
  uint64_t dest;
  read(src, &dest, sizeof(uint64_t));
  return le64toh(dest);
}

/**
 * @brief big endian float32
 */
void enc_bef32(stream_t dest, float src) {
  uint32_t tmp = htobe32((uint32_t) src);
  write(dest, &tmp, sizeof(uint32_t));
}
float dec_bef32(stream_t src) {
  uint32_t dest;
  read(src, &dest, sizeof(uint32_t));
  return (float) be32toh(dest);
}

/**
 * @brief little endian float32
 */
void enc_lef32(stream_t dest, float src) {
  uint32_t tmp = htole32((uint32_t) src);
  write(dest, &tmp, sizeof(uint32_t));
}
float dec_lef32(stream_t src) {
  uint32_t dest;
  read(src, &dest, sizeof(uint32_t));
  return (float) le32toh(dest);
}

/**
 * @brief big endian float64
 */
void enc_bef64(stream_t dest, double src) {
  uint64_t tmp = htobe64((uint64_t) src);
  write(dest, &tmp, sizeof(uint64_t));
}
double dec_bef64(stream_t src) {
  uint64_t dest;
  read(src, &dest, sizeof(uint64_t));
  return (double) be32toh(dest);
}

/**
 * @brief little endian float64
 */
void enc_lef64(stream_t dest, double src) {
  uint64_t tmp = htole64((uint64_t) src);
  write(dest, &tmp, sizeof(uint64_t));
}
double dec_lef64(stream_t src) {
  uint64_t dest;
  read(src, &dest, sizeof(uint64_t));
  return (double) le64toh(dest);
}

/**
 * @brief varnum parsing utilities
 */
int verify_varnum(const char *buf, size_t max_len, int type_max) {
  if(!max_len)
    return VARNUM_OVERRUN;
  int len =  1;
  for(; *(const unsigned char*) buf & 0x80; buf++, len++) {
    if(len == type_max)
      return VARNUM_INVALID;
    if((size_t) len == max_len)
      return VARNUM_OVERRUN;
  }
  return len;
}
int verify_varint(const char *buf, size_t max_len) {
  return verify_varnum(buf, max_len, 5);
}
int verify_varlong(const char *buf, size_t max_len) {
  return verify_varnum(buf, max_len, 10);
}

/**
 * @brief get varnum size
 */
size_t size_varint(uint32_t varint) {
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
size_t size_varlong(uint64_t varlong) {
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
 * @brief variable size integer
 */
void enc_varint(stream_t dest, uint64_t src) {
  uint8_t tmp;
  for(; src >= 0x80; src >>= 7) {
    tmp = 0x80 | (src & 0x7F);
    write(dest, &tmp, sizeof(uint8_t));
  }
  tmp = src & 0x7F;
  write(dest, &tmp, sizeof(uint8_t));
}
int64_t dec_varint(stream_t src) {
  uint64_t dest = 0;
  int i = 0;
  uint64_t j;
  read(src, &j, sizeof(uint8_t));
  for(; j & 0x80; i+=7) {
    read(src, &j, sizeof(uint8_t));
    dest |= (j & 0x7F) << i;
  }
  dest |= j << i;
  return (int64_t) dest;
}

/* todo:
 * more informative comments here
 * create packet constructor, write length byte and packet id byte before packet
 */

/**
 * @brief string
 */
void enc_string(stream_t dest, const char* src) {
  size_t length = strlen(src);
  enc_varint(dest, length);
  write(dest, src, length);
}
MALLOC char* dec_string(stream_t src) {
  size_t length = (size_t) dec_varint(src);
  char* string = malloc(length + 1);
  read(src, string, length);
  string[length] = 0;
  return string;
}

/**
 * @brief buffer
 */
void enc_buffer(stream_t dest, const char_vector_t src) {
  write(dest, src.data, src.size);
}
MALLOC char_vector_t dec_buffer(stream_t src, size_t len) {
  char_vector_t dest;
  dest.size = len;
  dest.data = malloc(sizeof(char) * len);
  read(src, dest.data, dest.size);
  return dest;
}
