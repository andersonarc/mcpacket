/**
 * @file connection.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief packet input/output functions implemenatations
 * @version 0.2
 * @date 2021-03-06
 */
    /* includes */
#include "mcp/connection.h" /* this */
#include "mcp/handler.h"    /* packet handlers */
#include "mcp/codec.h"      /* encoders */
#include "csafe/logf.h"     /* formatted logging */
#include <stdlib.h>         /* realloc */
#ifdef MCP_USE_ZLIB
    #include <zlib.h>
#else
    #include <libdeflate.h>
#endif /* MCP_USE_ZLIB */

    /* functions */
/**
 * @brief interface for receiving packet
 * 
 * @param context connection context
 */
void mcp_receive(mcp_context_t* context) {
    size_t length = mcp_decode_stream_varint(context->buffer.stream);
    if (context->compression_threshold > 0) {
        size_t uncompressed_size = mcp_decode_stream_varint(context->buffer.stream);
        size_t compressed_size = length - mcp_length_varlong(uncompressed_size);
        if (uncompressed_size == 0) {
            mcp_buffer_allocate(&context->buffer, compressed_size);
            mcp_buffer_init(&context->buffer);
        } else {
            char* compressed = malloc(compressed_size);
            assertd_not_null("mcp_receive", compressed);
            mcp_stream_read(context->buffer.stream, compressed, compressed_size);
            mcp_buffer_allocate(&context->buffer, uncompressed_size);
            #ifdef MCP_USE_ZLIB
                uncompress((Bytef*) context->buffer.data, (uLongf*) &uncompressed_size, (Bytef*) compressed, (uLong) compressed_size);
            #else
                struct libdeflate_decompressor* decompressor = libdeflate_alloc_decompressor();
                libdeflate_zlib_decompress(decompressor, compressed, compressed_size, context->buffer.data, context->buffer.size, NULL);
                libdeflate_free_decompressor(decompressor);
            #endif /* MCP_USE_ZLIB */
            free(compressed);
        }
    } else {
        mcp_buffer_allocate(&context->buffer, length);
        mcp_buffer_init(&context->buffer);
    }
    #ifdef NDEBUG
        mcp_handler_t* handler = mcp_handler_get(context->state, context->source, mcp_decode_varint(&context->buffer));
    #else
        mcp_packet_id_t id = mcp_decode_varint(&context->buffer);
        logd_f("mcp_receive", "packet %u::%s with length %zu", id, mcp_protocol_cstrings[context->state][context->source][id], length);
        mcp_handler_t* handler = mcp_handler_get(context->state, context->source, id);
    #endif /* NDEBUG */
    handler(context);
    mcp_buffer_free(&context->buffer);
}

/**
 * @brief interface for sending packets
 * 
 * @param context connection context with filled buffer
 */
void mcp_send(mcp_context_t* context) {
    if (context->compression_threshold > 0) {
        if (context->buffer.size > context->compression_threshold) {
            #ifdef MCP_USE_ZLIB
                size_t compressed_size = compressBound(context->buffer.size);
            #else
                struct libdeflate_compressor* compressor = libdeflate_alloc_compressor(6);
                size_t compressed_size = libdeflate_zlib_compress_bound(compressor, context->buffer.size);
            #endif /* MCP_USE_ZLIB */
            char* compressed = malloc(compressed_size);
            assertd_not_null("mcp_receive", compressed);
            #ifdef MCP_USE_ZLIB
                compress((Bytef*) compressed, (uLongf*) &compressed_size, (Bytef*) context->buffer.data, (uLong) context->buffer.size);
            #else
                libdeflate_zlib_compress(compressor, context->buffer.data, context->buffer.size, compressed, compressed_size);
                libdeflate_free_compressor(compressor);
            #endif /* MCP_USE_ZLIB */
            mcp_encode_stream_varint(compressed_size + mcp_length_varlong(context->buffer.size), context->buffer.stream);
            mcp_encode_stream_varint(context->buffer.size, context->buffer.stream);
            mcp_buffer_free(&context->buffer);
            mcp_buffer_set(&context->buffer, realloc(compressed, compressed_size), compressed_size);
        } else {
            mcp_encode_stream_varint(context->buffer.size + mcp_length_varlong(0), context->buffer.stream);
            mcp_encode_stream_varint(0, context->buffer.stream);
        }
    } else {
        mcp_encode_stream_varint(context->buffer.size, context->buffer.stream);
    }
    mcp_buffer_flush(&context->buffer);
    mcp_buffer_free(&context->buffer);
}