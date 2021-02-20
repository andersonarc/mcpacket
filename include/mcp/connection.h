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
#include <stdint.h>        /* integer types */
#include "mcp/io/buffer.h" /* buffered io */

    /* typedefs */
/**
 * @brief packet source enum
 */
typedef enum mcp_source_t {
    MCP_SOURCE_CLIENT,
    MCP_SOURCE_SERVER,
    MCP_SOURCE__MAX
} mcp_source_t;

/**
 * @brief packet state type
 */
typedef enum mcp_state_t {
    MCP_STATE_HANDSHAKING,
    MCP_STATE_STATUS,
    MCP_STATE_LOGIN,
    MCP_STATE_PLAY,
    MCP_STATE__MAX
} mcp_state_t;

/**
 * @brief packet type
 */
typedef struct mcp_packet_t {
    mcp_state_t state;
    mcp_source_t source;
    int id;
    char* name;
} mcp_packet_t;

/**
 * @brief server structure
 */
typedef struct mcp_server_t {
    char* hostname;
    uint16_t port;
} mcp_server_t;

/**
 * @brief client structure
 */
typedef struct mcp_client_t {
    char* username;
} mcp_client_t;

/**
 * @brief connection context
 */
typedef struct mcp_context_t {
    mcp_server_t server;
    mcp_client_t client;
    mcp_buffer_t buffer;
    mcp_state_t state;
    mcp_source_t source;
} mcp_context_t;

/**
 * @brief packet handler type
 */
typedef void mcp_handler_t(mcp_context_t* context);

#endif /* MCP_CONNECTION_H */