/**
 * @file codec.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief encoders/decoders for minecraft data types 
 * @version 0.5
 * @date 2021-02-07
 */
    /* header guard */
#ifndef MCP_CODEC_H
#define MCP_CODEC_H

    /* includes */
#include "mcp/type.h"   /* type definitions */
#include "mcp/stream.h" /* stream io */

    /* functions */
/**
 * @brief minecraft uuid
 */
void mcp_type_UUID_encode(mcp_type_UUID* this, stream_t dest);
void mcp_type_UUID_decode(mcp_type_UUID* this, stream_t src); 

/**
 * @brief minecraft position
 */
void mcp_type_Position_encode(mcp_type_Position* this, stream_t dest);
void mcp_type_Position_decode(mcp_type_Position* this, stream_t src);

/**
 * @brief minecraft slot
 */
void mcp_type_Slot_encode(mcp_type_Slot* this, stream_t dest);
void mcp_type_Slot_decode(mcp_type_Slot* this, stream_t src);

/**
 * @brief minecraft particle
 */
void mcp_type_Particle_encode(mcp_type_Particle* this, stream_t dest);
void mcp_type_Particle_decode(mcp_type_Particle* this, mcp_type_ParticleType p_type, stream_t src);

/**
 * @brief minecraft smelting
 */
void mcp_type_Smelting_encode(mcp_type_Smelting* this, stream_t dest);
void mcp_type_Smelting_decode(mcp_type_Smelting* this, stream_t src);

/**
 * @brief minecraft tag
 */
void mcp_type_Tag_encode(mcp_type_Tag* this, stream_t dest);
void mcp_type_Tag_decode(mcp_type_Tag* this, stream_t src);

/**
 * @brief minecraft entity equipment
 */
void mcp_type_EntityEquipment_encode(mcp_type_EntityEquipment* this, stream_t dest);
void mcp_type_EntityEquipment_decode(mcp_type_EntityEquipment* this, stream_t src);

/**
 * @brief minecraft entity metadata
 */
void mcp_type_EntityMetadata_encode(mcp_type_EntityMetadata* this, stream_t dest);
void mcp_type_EntityMetadata_decode(mcp_type_EntityMetadata* this, stream_t src);

/**
 * @brief byte
 */
void mcp_byte_encode(uint8_t* this, stream_t dest);
void mcp_byte_decode(uint8_t* this, stream_t src);

/**
 * @brief big endian uint16
 */
void mcp_be16_encode(uint16_t* this, stream_t dest);
void mcp_be16_decode(uint16_t* this, stream_t src);

/**
 * @brief little endian uint16
 */
void mcp_le16_encode(uint16_t* this, stream_t dest);
void mcp_le16_decode(uint16_t* this, stream_t src);

/**
 * @brief big endian uint32
 */
void mcp_be32_encode(uint32_t* this, stream_t dest);
void mcp_be32_decode(uint32_t* this, stream_t src);

/**
 * @brief little endian uint32
 */
void mcp_le32_encode(uint32_t* this, stream_t dest);
void mcp_le32_decode(uint32_t* this, stream_t src);

/**
 * @brief big endian uint64
 */
void mcp_be64_encode(uint64_t* this, stream_t dest);
void mcp_be64_decode(uint64_t* this, stream_t src);

/**
 * @brief little endian uint64
 */
void mcp_le64_encode(uint64_t* this, stream_t dest);
void mcp_le64_decode(uint64_t* this, stream_t src);

/**
 * @brief big endian float32
 */
void mcp_bef32_encode(float* this, stream_t dest);
void mcp_bef32_decode(float* this, stream_t src);

/**
 * @brief little endian float32
 */
void mcp_lef32_encode(float* this, stream_t dest);
void mcp_lef32_decode(float* this, stream_t src);

/**
 * @brief big endian float64
 */
void mcp_bef64_encode(double* this, stream_t dest);
void mcp_bef64_decode(double* this, stream_t src);

/**
 * @brief little endian float64
 */
void mcp_lef64_encode(double* this, stream_t dest);
void mcp_lef64_decode(double* this, stream_t src);

/**
 * @brief string
 * 
 * @warning strings are encoded length-prefixed and without null terminator, 
 *            but decoder returns regular null-terminated string
 */
void mcp_string_encode(char** this, stream_t dest);
void mcp_string_decode(char** this, stream_t src);
size_t size_string(const char* src);

/**
 * @brief buffer
 */
void mcp_buffer_encode(char_vector_t* this, stream_t dest);
void mcp_buffer_decode(char_vector_t* this, size_t length, stream_t src);

/**
 * @brief variable sized integer encoder and decoder
 */
void mcp_varint_encode(uint64_t* this, stream_t dest);
void mcp_varint_decode(uint64_t* this, stream_t src);

#endif /* MCP_CODEC_H */