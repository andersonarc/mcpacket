/**
 * @file stream.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief buffered io
 * @version 0.1
 * @date 2021-02-08
 */
    /* header guard */
#ifndef MCP_BUFFER_H
#define MCP_BUFFER_H

    /* includes */
#include <stddef.h>     /* size_t */
#include "mcp/stream.h" /* stream io */
#include "mcp/misc.h"   /* miscellanous */

    /* typedefs */
/**
 * @brief buffer data type
 */
typedef struct buffer_t {
    size_t size;
    size_t index;
    char* data;
} buffer_t;

    /* functions */
/**
 * @brief initialize a buffer
 *
 * @param buffer pointer to the buffer
 * @param size   buffer size
 */
MALLOC void buffer_init(buffer_t* buffer, size_t size);

/**
 * @brief deinitialize a buffer
 *
 * @param buffer pointer to the buffer
 */
void buffer_deinit(buffer_t* buffer);

/**
 * @brief write to a buffer
 * 
 * @param buffer the buffer
 * @param src    data source
 * @param count  number of bytes to write
 */
void buffer_write(buffer_t buffer, void* src, size_t count);

/**
 * @brief read from a buffer
 * 
 * @param buffer the buffer
 * @param dest   data destination
 * @param count  number of bytes to write
 */
void buffer_read(buffer_t buffer, void* dest, size_t count);

/**
 * @brief read a stream into a buffer
 * 
 * @param buffer the buffer
 * @param src    stream source
 */
void stream_to_buffer(stream_t stream, buffer_t buffer);

    /* defines */
/**
 * @brief write a variable to a stream
 * 
 * @param buffer   the buffer
 * @param variable variable to write from
 */
#define buffer_write_variable(buffer, variable) buffer_write(buffer, &(variable), sizeof(variable))

/**
 * @brief read a variable from a stream
 * 
 * @param buffer   the buffer
 * @param variable variable to read into
 */
#define buffer_read_variable(buffer, variable) buffer_read(buffer, &(variable), sizeof(variable))

#endif /* MCP_BUFFER_H */