/**
 * @file io.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief packet input/output system
 * @version 0.1
 * @date 2021-02-07
 */
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
    mcpacket_state state;
    const string_t username; 
} mcpacket_t;

    /* defines */
#define write(packet) 