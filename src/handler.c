/**
 * @file handler.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief packet handlers
 * @version 0.6
 * @date 2021-01-29
 */
    /* includes */
#include <stdlib.h>      /* memory functions */
#include <zlib.h>        /* uncompress packets */
#include "mcp/handler.h" /* this */
#include "mcp/codec.h"   /* encoders/decoders */
#include "mcp/varnum.h"  /* varnum size */

    /* functions */
/**
 * @brief blank packet handler
 * 
 * @param context connection context
 */
void mcp_handler_Blank(mcp_context_t* context) { }