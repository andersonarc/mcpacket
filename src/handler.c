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
//todo includes in .c

    /* functions */
/**
 * @brief read a packet from stream and handle it with *globally* specified handler
 * 
 * @param src stream from which to read a packet
 */
void mcp_handle_packet_global(stream_t src, mcpacket_state state, mcpacket_source source) {
    size_t length = dec_varint(src);
    int id = dec_varint(src);
    
    mcpacket_handler* handler = mcp_get_packet_handler(state, source, id);
    handler(src, length);
}

/**
 * @brief read a packet from stream and handle it with specified handler
 * 
 * @param src stream from which to read a packet
 */
void mcp_handle_packet(stream_t src, mcpacket_handler* handler) {
    size_t length = dec_varint(src);
    dec_varint(src);
    handler(src, length);
}

/**
 * @brief blank packet handler
 * 
 * @param packet incoming packet
 */
void mcp_blank_handler(stream_t src, size_t length) {
    char* buffer = (char*) malloc(length);
    stream_read(src, buffer, length);
    free(buffer);
}