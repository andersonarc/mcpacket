/**
 * @file handler.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief packet handler system
 * @version 0.5
 * @date 2021-01-29
 */
    /* includes */
#include <stdlib.h>      /* memory functions */
#include <zlib.h>        /* uncompress packets */
#include "mcp/handler.h" /* this */
#include "mcp/codec.h"   /* encoders/decoders */
#include "mcp/varnum.h"  /* varnum size */

    /* functions */
/**
 * @brief read a packet from a buffered stream and handle it with a globally specified handler
 * 
 * @param context connection context
 * @param length full packet length
 */
void mcp_handler_execute_uncompressed(mcp_context_t* context, size_t length) {
    mcp_buffer_allocate(&context->buffer, length);
    mcp_buffer_init(&context->buffer);
    mcp_handler_t* handler = mcp_handler_get(context->state, context->source, mcp_decode_varint(&context->buffer));
    handler(context);
    mcp_buffer_free(&context->buffer);
}

/**
 * @brief read and decompress a packet from a buffered stream and handle it with a globally specified handler
 * 
 * @param context connection context
 * @param length full packet length
 */
void mcp_handler_execute_compressed(mcp_context_t* context, size_t length) {
    size_t data_size = mcp_decode_stream_varint(context->buffer.stream);
    size_t buffer_size = length - mcp_length_varlong(data_size);
    if (data_size == 0) {
        mcp_buffer_allocate(&context->buffer, buffer_size);
        mcp_buffer_init(&context->buffer);
    } else {
        char* compressed = malloc(sizeof(char*) * buffer_size);
        mcp_stream_read(context->buffer.stream, compressed, buffer_size);
        mcp_buffer_allocate(&context->buffer, data_size);
        uncompress((Bytef*) &context->buffer.data, (uLongf*) &data_size, (Bytef*) compressed, (uLong) buffer_size);
        free(compressed);
    }
    mcp_handler_t* handler = mcp_handler_get(context->state, context->source, mcp_decode_varint(&context->buffer));
    handler(context);
    mcp_buffer_free(&context->buffer);
}

/**
 * @brief blank packet handler
 * 
 * @param context connection context
 */
void mcp_handler_Blank(mcp_context_t* context) { }