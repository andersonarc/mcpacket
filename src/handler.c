/**
 * @file handler.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief packet handler system
 * @version 0.3
 * @date 2021-01-29
 */
    /* includes */
#include "mcp/handler.h"    /* this */
#include "mcp/protocol.h"   /* protocol */
#include "mcp/codec.h"      /* encoders/decoders */
//todo includes in .c

    /* functions */
/**
 * @brief read a packet from stream and handle it with *globally* specified handler
 * 
 * @param src stream from which to read a packet
 */
void mcp_handle_packet(stream_t src, mcpacket_state state, mcpacket_source source) {
    size_t length = mcp_varint_stream_decode(src);
    int id = mcp_varint_stream_decode(src);
    buffer_t buffer;
    buffer_init(&buffer, length);
    mcpacket_handler* handler = mcp_get_packet_handler(state, source, id);
    handler(buffer, length);
    buffer_deinit(&buffer);
}

/**
 * @brief blank packet handler
 * 
 * @param packet incoming packet
 */
void mcp_blank_handler(buffer_t src, size_t length) { }