/**
 * @file connection.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief connection structures and packet input/output functions
 * @version 0.2
 * @date 2021-02-07
 */
    /* header guard */
#ifndef MCP_CONNECTION_H
#define MCP_CONNECTION_H

    /* includes */
#include "mcp/io/buffer.h" /* buffered io */
#include "mcp/type.h"      /* data types */
#include <stdint.h>        /* integer types */

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
 * @brief packet id type
 */
typedef uint8_t mcp_packet_id_t;

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
    mcp_type_UUID uuid;
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
    int compression_threshold;
} mcp_context_t;

/**
 * @brief packet handler type
 */
typedef void mcp_handler_t(mcp_context_t* context);

    /* functions */
/**
 * @brief interface for receiving packets
 * 
 * @param context connection context
 */
void mcp_receive(mcp_context_t* context);

/**
 * @brief interface for sending packets
 * 
 * @param context connection context with filled buffer
 */
void mcp_send(mcp_context_t* context);

#endif /* MCP_CONNECTION_H */