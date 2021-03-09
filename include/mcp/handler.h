/**
 * @file handler.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief packet handler system
 * @version 0.6
 * @date 2021-01-29
 */
    /* header guard */
#ifndef MCP_HANDLER_H
#define MCP_HANDLER_H

    /* includes */
#include "mcp/protocol.h" /* protocol */
#include "csafe/assertd.h" /* debug assertions */

    /* functions */
/**
 * @brief get a handler for a packet
 * 
 * @param state connection state
 * @param source packet source
 * @param id packet id
 * 
 * @warning unsafe, throws segmentation fault on error/corrupts the stream silently when reading
 */
static inline mcp_handler_t* mcp_handler_get(mcp_state_t state, mcp_source_t source, mcp_packet_id_t id) {
    assertd_true_custom("mcp_handler_get", id < mcp_protocol_max_ids[state][source], "invalid packet id")
    return mcp_protocol_handlers[state][source][id];
}

/**
 * @brief set a handler for a packet
 * 
 * @param state connection state
 * @param source packet source
 * @param id packet id
 * 
 * @warning unsafe, throws segmentation fault on error/corrupts the stream silently when reading
 */
static inline void mcp_handler_set(mcp_state_t state, mcp_source_t source, mcp_packet_id_t id, mcp_handler_t* handler) {
    assertd_true_custom("mcp_handler_set", id < mcp_protocol_max_ids[state][source], "invalid packet id")
    mcp_protocol_handlers[state][source][id] = handler;
}

/**
 * @brief blank packet handler
 * 
 * @param context connection context
 */
void mcp_handler_Blank(mcp_context_t* context);

#endif /* MCP_HANDLER_H */