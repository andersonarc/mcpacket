/**
 * @file handler.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief packet handler system
 * @version 0.3
 * @date 2021-01-29
 */
    /* header guard */
#ifndef MCP_HANDLER_H
#define MCP_HANDLER_H

    /* includes */
#include "mcp/protocol.h" /* protocol */

    /* typedefs */
/**
 * @brief packet handler function
 */
typedef void mcp_packet_handler_t(void* packet);

/**
 * @brief generic packet
 */
typedef struct mcp_generic_packet {
    mcpacket_t packet;
} mcp_generic_packet;

    /* defines */
/**
 * @brief get a handler for a packet
 * 
 * @warning very unsafe, throws segmentation fault on error/corrupts the stream silently when reading
 */
#define mcp_get_packet_handler(state, source, id) mcp_protocol_handlers[state][source][id]

/**
 * @brief set a handler for a packet
 * 
 * @warning very unsafe, throws segmentation fault on error/corrupts the stream silently when reading
 */
#define mcp_set_packet_handler(state, source, id, handler) mcp_protocol_handlers[state][source][id] = handler

    /* functions */
/**
 * @brief read a packet from stream and handle it with *globally* specified handler
 * 
 * @param src stream from which to read a packet
 */
void mcp_handle_packet_global(stream_t src, mcpacket_state state, mcpacket_source source);

/**
 * @brief read a packet from stream and handle it with specified handler
 * 
 * @param src stream from which to read a packet
 */
void mcp_handle_packet(stream_t src, mcpacket_handler* handler);

/**
 * @brief default packet handler
 * 
 * @param packet incoming packet
 */
void mcp_blank_handler(stream_t src, size_t length);

/**
 * @brief packet handler for debugging
 * 
 * @param packet incoming packet
 */
void mcp_debug_handler(stream_t src, size_t length);

#endif /* MCP_HANDLER_H */