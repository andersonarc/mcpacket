/**
 * @file connection.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief connection structures
 * @version 0.1
 * @date 2021-02-07
 */
    /* header guard */
#ifndef MCP_CONNECTION_H
#define MCP_CONNECTION_H

    /* includes */
#include <stdint.h>       /* integer types */
#include "mcp/protocol.h" /* protocol */

    /* typedefs */
/**
 * @brief server structure
 */
typedef struct mcp_server_t {
    char* hostname;
    uint16_t port;
} mcp_server_t;

/**
 * @brief connection context
 */
typedef struct mcp_context_t {
    mcp_server_t server;
    mcp_state_t state;
    const string_t username; 
} mcp_context_t;

#endif /* MCP_CONNECTION_H */