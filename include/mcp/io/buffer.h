/**
 * @file buffer.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief buffered io stream
 * @version 0.3
 * @date 2021-02-08
 */
    /* header guard */
#ifndef MCP_IO_BUFFER_H
#define MCP_IO_BUFFER_H

    /* includes */
#include <stddef.h>        /* size_t */
#include "mcp/io/stream.h" /* stream io */
#include "mcp/misc.h"      /* annotations */

    /* typedefs */
/**
 * @brief buffered stream data type
 */
typedef struct mcp_buffer_t {
    size_t size;
    size_t index;
    mcp_stream_t stream;
    char* data;
} mcp_buffer_t;

    /* functions */
/**
 * @brief bind a buffer to a stream
 *
 * @param buffer pointer to the buffer
 * @param size   buffer size
 */
void mcp_buffer_bind(mcp_buffer_t* buffer, mcp_stream_t stream);

/**
 * @brief allocate a buffer
 *
 * @param buffer pointer to the buffer
 * @param size   buffer size
 */
MALLOC void mcp_buffer_allocate(mcp_buffer_t* buffer, size_t size); //todo return status

/**
 * @brief initialize a buffer
 * 
 * @param buffer pointer to the buffer
 */
void mcp_buffer_init(mcp_buffer_t* buffer);

/**
 * @brief free a buffer
 *
 * @param buffer pointer to the buffer
 * 
 * @warning it's the caller's responsibility to flush the buffer/close the stream
 */
void mcp_buffer_free(mcp_buffer_t* buffer); //todo return status

/**
 * @brief flush a buffer into bound stream
 * 
 * @param buffer the buffer
 */
void mcp_buffer_flush(mcp_buffer_t* buffer); //todo return status

/**
 * @brief write to a buffer
 * 
 * @param buffer pointer to the buffer
 * @param src    data source
 * @param count  number of bytes to write
 * 
 * @warning use with caution, segmentation fault is possible
 */
void mcp_buffer_write(mcp_buffer_t* buffer, void* src, size_t count); //todo return status?

/**
 * @brief read from a buffer
 * 
 * @param buffer pointer to the buffer
 * @param dest   data destination
 * @param count  number of bytes to write
 * 
 * @warning buffer overflow is possible
 * @warning use with caution, segmentation fault is possible
 */
void mcp_buffer_read(mcp_buffer_t* buffer, void* dest, size_t count); //todo return status?

    /* defines */
/**
 * @brief write a variable to a stream
 * 
 * @param buffer   pointer to the buffer
 * @param variable variable to write from
 */
#define mcp_buffer_write_variable(buffer, variable) mcp_buffer_write(buffer, &(variable), sizeof(variable))

/**
 * @brief read a variable from a stream
 * 
 * @param buffer   pointer to the buffer
 * @param variable variable to read into
 */
#define mcp_buffer_read_variable(buffer, variable) mcp_buffer_read(buffer, &(variable), sizeof(variable))

#endif /* MCP_IO_BUFFER_H */