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
 * @brief blank packet handler
 * 
 * @param buffer packet buffer
 */
void mcp_handler_Blank(mcp_buffer_t* buffer) { }