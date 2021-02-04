/**
 * @file handler.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief packet handler system
 * @version 0.1
 * @date 2021-01-29
 */
    /* includes */
#include "mcp/handler.h"    /* this */

/**
 * @brief blank packet handler
 * 
 * @param packet incoming packet
 */
void mcp_blank_handler(void* packet) { }

/**
 * @brief structure for debug only
 * //todo
 */
typedef struct mcp_debug_packet {
    //mcpacket_t packet;
} mcp_debug_packet;

/**
 * @brief packet handler for debugging
 * 
 * @param packet incoming packet
 */
void mcp_debug_handler(void* packet) {
    //mcp_debug_packet internal = *((mcp_debug_packet*) packet);
    //printf("mcpacket: mcp_debug_handler: received packet %s with id %d\n", internal.packet.name, internal.packet.id);
}
