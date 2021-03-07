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
#include "mcp/io/buffer.h" /* buffered io */
#include "mcp/type.h"      /* type definitions */

    /* functions */
/**
 * @brief minecraft uuid
 */
inline void mcp_encode_type_UUID(mcp_type_UUID* this, mcp_buffer_t* dest);
inline void mcp_decode_type_UUID(mcp_type_UUID* this, mcp_buffer_t* src); 

/**
 * @brief minecraft position
 */
inline void mcp_encode_type_Position(mcp_type_Position* this, mcp_buffer_t* dest);
inline void mcp_decode_type_Position(mcp_type_Position* this, mcp_buffer_t* src);

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
 * @brief byte
 */
inline void mcp_encode_byte(uint8_t* this, mcp_buffer_t* dest);
inline void mcp_decode_byte(uint8_t* this, mcp_buffer_t* src);

/**
 * @brief big endian uint16
 */
inline void mcp_encode_be16(uint16_t* this, mcp_buffer_t* dest);
inline void mcp_decode_be16(uint16_t* this, mcp_buffer_t* src);

/**
 * @brief little endian uint16
 */
inline void mcp_encode_le16(uint16_t* this, mcp_buffer_t* dest);
inline void mcp_decode_le16(uint16_t* this, mcp_buffer_t* src);

/**
 * @brief big endian uint32
 */
inline void mcp_encode_be32(uint32_t* this, mcp_buffer_t* dest);
inline void mcp_decode_be32(uint32_t* this, mcp_buffer_t* src);

/**
 * @brief little endian uint32
 */
inline void mcp_encode_le32(uint32_t* this, mcp_buffer_t* dest);
inline void mcp_decode_le32(uint32_t* this, mcp_buffer_t* src);

/**
 * @brief big endian uint64
 */
inline void mcp_encode_be64(uint64_t* this, mcp_buffer_t* dest);
inline void mcp_decode_be64(uint64_t* this, mcp_buffer_t* src);

/**
 * @brief little endian uint64
 */
inline void mcp_encode_le64(uint64_t* this, mcp_buffer_t* dest);
inline void mcp_decode_le64(uint64_t* this, mcp_buffer_t* src);

/**
 * @brief big endian float32
 */
inline void mcp_encode_bef32(float* this, mcp_buffer_t* dest);
inline void mcp_decode_bef32(float* this, mcp_buffer_t* src);

/**
 * @brief little endian float32
 */
inline void mcp_encode_lef32(float* this, mcp_buffer_t* dest);
inline void mcp_decode_lef32(float* this, mcp_buffer_t* src);

/**
 * @brief big endian float64
 */
inline void mcp_encode_bef64(double* this, mcp_buffer_t* dest);
inline void mcp_decode_bef64(double* this, mcp_buffer_t* src);

/**
 * @brief little endian float64
 */
inline void mcp_encode_lef64(double* this, mcp_buffer_t* dest);
inline void mcp_decode_lef64(double* this, mcp_buffer_t* src);

/**
 * @brief string
 * 
 * @warning strings are encoded length-prefixed and without null terminator, 
 *            but decoder returns regular null-terminated string
 */
void mcp_encode_string(char** this, mcp_buffer_t* dest);
void mcp_decode_string(char** this, mcp_buffer_t* src);
inline size_t mcp_length_string(const char* src);

/**
 * @brief buffer
 */
inline void mcp_encode_buffer(char_vector_t* this, mcp_buffer_t* dest);
inline void mcp_decode_buffer(char_vector_t* this, size_t length, mcp_buffer_t* src);

/**
 * @brief variable sized integer
 */
void mcp_encode_varint(uint64_t this, mcp_buffer_t* dest);
uint64_t mcp_decode_varint(mcp_buffer_t* src);

/**
 * @brief variable sized number (from stream)
 */
void mcp_encode_stream_varint(uint64_t src, mcp_stream_t dest);
uint64_t mcp_decode_stream_varint(mcp_stream_t src);
inline size_t mcp_length_varint(uint32_t varint);
inline size_t mcp_length_varlong(uint64_t varlong);

/**
 * @brief nbt stub
 */
void mcp_encode_type_NbtTagCompound(mcp_type_NbtTagCompound* this, mcp_buffer_t* dest);
void mcp_decode_type_NbtTagCompound(mcp_type_NbtTagCompound* this, mcp_buffer_t* src);
void mcp_read_type_NbtTagCompound(mcp_type_NbtTagCompound* this, mcp_buffer_t* src);

#endif /* MCP_CODEC_H */