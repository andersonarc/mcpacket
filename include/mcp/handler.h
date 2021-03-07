/**
 * @file handler.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief packet handler system
 * @version 0.5
 * @date 2021-01-29
 */
    /* header guard */
#ifndef MCP_HANDLER_H
#define MCP_HANDLER_H

    /* includes */
#include "mcp/protocol.h"   /* protocol */

    /* defines */
    //todo safe debug variants for these
/**
 * @brief get a handler for a packet
 * 
 * @warning unsafe, throws segmentation fault on error/corrupts the stream silently when reading
 */
#define mcp_handler_get(state, source, id) mcp_protocol_handlers[state][source][id]

/**
 * @brief set a handler for a packet
 * 
 * @warning unsafe, throws segmentation fault on error/corrupts the stream silently when reading
 */
#define mcp_handler_set(state, source, id, handler) mcp_protocol_handlers[state][source][id] = handler

    /* functions */
/**
 * @brief blank packet handler
 * 
 * @param context connection context
 */
void mcp_handler_Blank(mcp_context_t* context);

#endif /* MCP_HANDLER_H */