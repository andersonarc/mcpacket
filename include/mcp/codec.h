/**
 * @file codec.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief encoders/decoders for minecraft data types 
 * @version 0.7
 * @date 2021-02-07
 */
    /* header guard */
#ifndef MCP_CODEC_H
#define MCP_CODEC_H

    /* includes */
#include <endian.h> /* byte swap */ 
#include "mcp/io/buffer.h" /* buffered io */
#include "mcp/type.h"      /* type definitions */

    /* functions */
/**
 * @brief minecraft uuid
 */
void mcp_encode_type_UUID(mcp_type_UUID* this, mcp_buffer_t* dest);
void mcp_decode_type_UUID(mcp_type_UUID* this, mcp_buffer_t* src); 

/**
 * @brief minecraft position
 */
void mcp_encode_type_Position(mcp_type_Position* this, mcp_buffer_t* dest);
void mcp_decode_type_Position(mcp_type_Position* this, mcp_buffer_t* src);

/**
 * @brief minecraft slot
 */
void mcp_encode_type_Slot(mcp_type_Slot* this, mcp_buffer_t* dest);
void mcp_decode_type_Slot(mcp_type_Slot* this, mcp_buffer_t* src);

/**
 * @brief minecraft particle
 */
void mcp_encode_type_Particle(mcp_type_Particle* this, mcp_buffer_t* dest);
void mcp_decode_type_Particle(mcp_type_Particle* this, mcp_type_ParticleType p_type, mcp_buffer_t* src);

/**
 * @brief minecraft smelting
 */
void mcp_encode_type_Smelting(mcp_type_Smelting* this, mcp_buffer_t* dest);
void mcp_decode_type_Smelting(mcp_type_Smelting* this, mcp_buffer_t* src);

/**
 * @brief minecraft tag
 */
void mcp_encode_type_Tag(mcp_type_Tag* this, mcp_buffer_t* dest);
void mcp_decode_type_Tag(mcp_type_Tag* this, mcp_buffer_t* src);

/**
 * @brief minecraft entity equipment
 */
void mcp_encode_type_EntityEquipment(mcp_type_EntityEquipment* this, mcp_buffer_t* dest);
void mcp_decode_type_EntityEquipment(mcp_type_EntityEquipment* this, mcp_buffer_t* src);

/**
 * @brief minecraft entity metadata
 */
void mcp_encode_type_EntityMetadata(mcp_type_EntityMetadata* this, mcp_buffer_t* dest);
void mcp_decode_type_EntityMetadata(mcp_type_EntityMetadata* this, mcp_buffer_t* src);

/**
 * @brief utility macro for creating simple encoders/decoders
 */
#define __mcp_number(type, postfix, encode_converter, decode_converter) \
static inline void mcp_encode_##postfix(type* this, mcp_buffer_t* dest) {    \
  mcp_buffer_get_pointer(dest, type) = encode_converter(*this);       \
}                                                                       \
static inline void mcp_decode_##postfix(type* this, mcp_buffer_t* src) {     \
  *this = decode_converter(mcp_buffer_get_pointer(src, type));        \
}   

#define __mcp_number_a(type, actual, postfix, encode_converter, decode_converter) \
static inline void mcp_encode_##postfix(type* this, mcp_buffer_t* dest) {              \
  mcp_buffer_get_pointer(dest, actual) = encode_converter((actual) *this);      \
}                                                                                 \
static inline void mcp_decode_##postfix(type* this, mcp_buffer_t* src) {               \
  *this = (type) decode_converter(mcp_buffer_get_pointer(src, actual));         \
}   


#define __mcp_dummy_converter(x) x

/**
 * @brief byte
 */
//todo push values, not pointers in encoders
__mcp_number(uint8_t, byte, __mcp_dummy_converter, __mcp_dummy_converter)

/**
 * @brief big endian uint16
 */
__mcp_number(uint16_t, be16, htobe16, be16toh)

/**
 * @brief little endian uint16
 */
__mcp_number(uint16_t, le16, htole16, le16toh)

/**
 * @brief big endian uint32
 */
__mcp_number(uint32_t, be32, htobe32, be32toh)

/**
 * @brief little endian uint32
 */
__mcp_number(uint32_t, le32, htole32, le32toh)

/**
 * @brief big endian uint64
 */
__mcp_number(uint64_t, be64, htobe64, be64toh)

/**
 * @brief little endian uint64
 */
__mcp_number(uint64_t, le64, htole64, le64toh)

/**
 * @brief big endian float32
 */
__mcp_number_a(float, uint32_t, bef32, htobe32, be32toh)

/**
 * @brief little endian float32
 */
__mcp_number_a(float, uint32_t, lef32, htole32, le32toh)

/**
 * @brief big endian float64
 */
__mcp_number_a(double, uint64_t, bef64, htobe64, be64toh)

/**
 * @brief little endian float64
 */
__mcp_number_a(double, uint64_t, lef64, htole64, le64toh)

/**
 * @brief undefine the utility macro
 */
#undef __mcp_number
#undef __mcp_number_a
#undef __mcp_dummy_converter

/**
 * @brief variable sized number (from stream)
 */
void mcp_encode_stream_varint(uint64_t src, mcp_stream_t dest);
uint64_t mcp_decode_stream_varint(mcp_stream_t src);

/**
 * @brief calculate varnum size for fixed integer
 */
static inline size_t mcp_length_varint(uint32_t varint) {
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
static inline size_t mcp_length_varlong(uint64_t varlong) {
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
 * @brief string
 * 
 * @warning strings are encoded length-prefixed and without null terminator, 
 *            but decoder returns regular null-terminated string
 */
void mcp_encode_string(char** this, mcp_buffer_t* dest);
void mcp_decode_string(char** this, mcp_buffer_t* src);
static inline size_t mcp_length_string(const char* src) {
  size_t length = strlen(src);
  return mcp_length_varlong(length) + length;
}

/**
 * @brief buffer
 */
static inline void mcp_encode_buffer(char_vector_t* this, mcp_buffer_t* dest) {
  mcp_buffer_write(dest, this->data, this->size);
}
static inline void mcp_decode_buffer(char_vector_t* this, size_t length, mcp_buffer_t* src) {
  this->size = length;
  this->data = malloc(sizeof(char) * length);
  mcp_buffer_read(src, this->data, this->size);
}

/**
 * @brief variable sized integer
 */
void mcp_encode_varint(uint64_t this, mcp_buffer_t* dest);
uint64_t mcp_decode_varint(mcp_buffer_t* src);

/**
 * @brief nbt stub
 */
void mcp_encode_type_NbtTagCompound(mcp_type_NbtTagCompound* this, mcp_buffer_t* dest);
void mcp_decode_type_NbtTagCompound(mcp_type_NbtTagCompound* this, mcp_buffer_t* src);
void mcp_read_type_NbtTagCompound(mcp_type_NbtTagCompound* this, mcp_buffer_t* src);

#endif /* MCP_CODEC_H */