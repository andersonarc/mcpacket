/**
 * @file connection.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief connection structures
 * @version 0.2
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
 * @brief server info structure
 */
typedef struct mcp_server_info_t {
    char* hostname;
    uint16_t port;
} mcp_server_info_t;

/**
 * @brief server structure
 */
typedef struct mcp_server_t {
    mcp_server_info_t info;
    buffer_t buffer;
} mcp_server_t;

/**
 * @brief connection context
 */
typedef struct mcp_context_t {
    mcp_server_t server;
    mcp_state_t state;
    const char* username; 
} mcp_context_t;

#endif /* MCP_CONNECTION_H */