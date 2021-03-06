/**
 * @file connection.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief packet input/output functions implemenatation
 * @version 0.1
 * @date 2021-03-06
 */
    /* includes */
#include "mcp/connection.h" /* this */
#include "mcp/handler.h"    /* packet handlers */
#include "mcp/codec.h"      /* encoders */
#include "mcp/varnum.h"     /* varnum length */
#include <stdlib.h>         /* realloc */

    /* functions */
/**
 * @brief interface for receiving packet
 * 
 * @param context connection context
 */
void mcp_receive(mcp_context_t* context) {
    size_t length = mcp_decode_stream_varint(context->buffer.stream);
    if (context->compression_threshold > 0) {
        mcp_handler_execute_compressed(context, length);
    } else {
        mcp_handler_execute_uncompressed(context, length);
    }
}

/**
 * @brief interface for sending packets
 * 
 * @param context connection context with filled buffer
 */
void mcp_send(mcp_context_t* context) {
    if (context->compression_threshold > 0) {
        if (context->buffer.size > context->compression_threshold) {
            size_t compressed_size = compressBound(context->buffer.size);
            char* compressed = malloc(sizeof(char*) * compressed_size);
            compress((Bytef*) compressed, (uLongf*) &compressed_size, (Bytef*) context->buffer.data, (uLong) context->buffer.size);
            mcp_encode_stream_varint(context->buffer.stream, compressed_size + mcp_length_varlong(context->buffer.size));
            mcp_encode_stream_varint(context->buffer.stream, context->buffer.size);
            mcp_buffer_free(&context->buffer);
            mcp_buffer_set(&context->buffer, realloc(compressed, compressed_size), compressed_size);
        } else {
            mcp_encode_stream_varint(context->buffer.stream, context->buffer.size + mcp_length_varlong(0));
            mcp_encode_stream_varint(context->buffer.stream, 0);
        }
    } else {
        mcp_encode_stream_varint(context->buffer.size, context->buffer.stream);
    }
    mcp_buffer_flush(&context->buffer);
    mcp_buffer_free(&context->buffer);
}