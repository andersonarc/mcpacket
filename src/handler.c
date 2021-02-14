/**
 * @file handler.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief packet handler system
 * @version 0.5
 * @date 2021-01-29
 */
    /* includes */
#include "mcp/handler.h"    /* this */
#include "mcp/codec.h"      /* encoders/decoders */

    /* functions */
/**
 * @brief read a packet from the stream and handle it with a globally specified handler
 * 
 * @param stream the stream
 */
void mcp_handler_execute(stream_t stream, mcp_state_t state, mcp_source_t source) {
    size_t length = mcp_varint_stream_decode(stream);
    buffer_t buffer;
    buffer_bind(&buffer, stream);
    buffer_init(&buffer, length);
    int id = mcp_varint_decode(&buffer);
    mcp_handler_t* handler = mcp_handler_get(state, source, id);
    handler(&buffer);
    buffer_deinit(&buffer);
}

/**
 * @brief blank packet handler
 * 
 * @param buffer packet buffer
 */
void mcp_handler_blank(buffer_t* buffer) { }