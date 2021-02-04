/**
 * @file handler.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief packet handler system
 * @version 0.1
 * @date 2021-01-29
 */
    /* header guard */
#ifndef MCP_HANDLER_H
#define MCP_HANDLER_H

    /* includes */
#include <stdio.h>          /* io for debug */
//#include "mcp/protocol.h"   /* protocol */

/**
 * @brief packet handler function
 */
typedef void mcp_packet_handler_t(void* packet);

/**
 * @brief default packet handler
 * 
 * @param packet incoming packet
 */
void mcp_blank_handler(void* packet);

/**
 * @brief packet handler for debugging
 * 
 * @param packet incoming packet
 */
void mcp_debug_handler(void* packet);

#endif /* MCP_HANDLER_H */