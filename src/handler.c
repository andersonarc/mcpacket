/**
 * @file handler.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief packet handler system
 * @version 0.5
 * @date 2021-01-29
 */
    /* includes */
#include "mcp/handler.h" /* this */
#include "mcp/codec.h"   /* encoders/decoders */
#include "mcp/varnum.h"  /* varnum size */
#include <stdlib.h>      /* memory functions */
#include <zlib.h>        /* uncompress packets */

    /* functions */
/**
 * @brief read a packet from a buffered stream and handle it with a globally specified handler
 * 
 * @param buffer the buffer
 */
void mcp_handler_execute(mcp_buffer_t* buffer, mcp_state_t state, mcp_source_t source) {
    mcp_buffer_allocate(buffer, mcp_decode_stream_varint(buffer->stream));
    mcp_buffer_init(buffer);
    mcp_handler_t* handler = mcp_handler_get(state, source, mcp_decode_varint(buffer));
    handler(buffer);
    mcp_buffer_free(buffer);
}

/**
 * @brief read and decompress a packet from a buffered stream and handle it with a globally specified handler
 * 
 * @param buffer the buffer
 */
void mcp_handler_compressed_execute(mcp_buffer_t* buffer, mcp_state_t state, mcp_source_t source) {
    size_t full_size = mcp_decode_stream_varint(buffer->stream);
    size_t uncompressed_size = mcp_decode_stream_varint(buffer->stream);
    size_t compressed_size = full_size - size_varlong(uncompressed_size);
    char* compressed = malloc(sizeof(char*) * compressed_size);
    mcp_stream_read(buffer->stream, compressed, compressed_size);

    mcp_buffer_allocate(buffer, uncompressed_size);
    uncompress((Bytef*) buffer->data, (uLongf*) &uncompressed_size, (Bytef*) compressed, (uLong) compressed_size);
    mcp_handler_t* handler = mcp_handler_get(state, source, mcp_decode_varint(buffer));
    handler(buffer);
    mcp_buffer_free(buffer);

    free(compressed);
}

/**
 * @brief blank packet handler
 * 
 * @param buffer packet buffer
 */
void mcp_handler_Blank(mcp_buffer_t* buffer) { }