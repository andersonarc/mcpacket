/**
 * @file handler.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief packet handler system
 * @version 0.4
 * @date 2021-01-29
 */
    /* header guard */
#ifndef MCP_HANDLER_H
#define MCP_HANDLER_H

    /* includes */
#include "mcp/protocol.h" /* protocol */

    /* defines */
/**
 * @brief get a handler for a packet
 * 
 * @warning very unsafe, throws segmentation fault on error/corrupts the stream silently when reading
 */
#define mcp_handler_get(state, source, id) mcp_protocol_handlers[state][source][id]

/**
 * @brief set a handler for a packet
 * 
 * @warning very unsafe, throws segmentation fault on error/corrupts the stream silently when reading
 */
#define mcp_handler_set(state, source, id, handler) mcp_protocol_handlers[state][source][id] = handler

    /* functions */
/**
 * @brief read a packet from the stream and handle it with a globally specified handler
 * 
 * @param stream the stream
 */
void mcp_handler_execute(stream_t stream, mcp_state_t state, mcp_source_t source);

/**
 * @brief blank packet handler
 * 
 * @param buffer packet buffer
 */
void mcp_handler_blank(buffer_t* buffer);

#endif /* MCP_HANDLER_H */