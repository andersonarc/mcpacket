/**
 * @file handler.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief packet handler system
 * @version 0.1
 * @date 2021-01-29
 */
    /* includes */
#include "mcp/handler.h"    /* this */
#include "mcp/protocol.h"   /* protocol */


    /* functions */ // todo refactor subheaders
/**
 * @brief read a packet from stream and handle it with *globally* specified handler
 * 
 * @param src stream from which to read a packet
 */
void mcp_handle_packet_global(stream_t src, mcpacket_state state, mcpacket_source source) {
    size_t length = dec_varint(src);
    int id = dec_varint(src);
    char* buffer = (char*) malloc(sizeof(char) * (length - size_varint(id)));
    mcpacket_handler* handler = mcp_get_packet_handler(state, source, id);
    handler(buffer, length);
    free(buffer);
}

/**
 * @brief read a packet from stream and handle it with specified handler
 * 
 * @param src stream from which to read a packet
 */
void mcp_handle_packet(stream_t src, mcpacket_handler* handler) {
    size_t length = dec_varint(src);
    int id = dec_varint(src);
    char* buffer = (char*) malloc(sizeof(char) * (length - size_varint(id)));
    handler(buffer, length);
    free(buffer);
}

/**
 * @brief blank packet handler
 * 
 * @param packet incoming packet
 */
void mcp_blank_handler(void* packet, size_t length) { }

/**
 * @brief packet handler for debugging
 * 
 * @param packet incoming packet
 */
void mcp_debug_handler(void* packet, size_t length) {
    mcp_generic_packet internal = *((mcp_generic_packet*) packet);
    printf("mcpacket: mcp_debug_handler: received packet %s with id %d\n", internal.packet.name, internal.packet.id);
}
