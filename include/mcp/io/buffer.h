/**
 * @file buffer.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief buffered io
 * @version 0.6
 * @date 2021-02-08
 */
    /* header guard */
#ifndef MCP_IO_BUFFER_H
#define MCP_IO_BUFFER_H

    /* includes */
#include "mcp/io/stream.h" /* stream io */
#include <stddef.h>        /* size_t */
#include <stdlib.h>        /* memory functions */

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
 * @brief get a pointer to a current index in a buffer
 * 
 * @param buffer pointer to the buffer
 * 
 * @warning buffer index should be increased after modification
 * @warning wrong usage could lead to buffer overflow
 */
static inline char* mcp_buffer_current(mcp_buffer_t* buffer) {
    return &buffer->data[buffer->index];
}

/**
 * @brief increase buffer's index
 * 
 * @param buffer pointer to the buffer
 * @param count  number of bytes to increase for
 */
static inline void mcp_buffer_increment(mcp_buffer_t* buffer, size_t count) {
    buffer->index += count;
}

/**
 * @brief bind a buffer to a stream
 *
 * @param buffer pointer to the buffer
 * @param size   buffer size
 */
static inline void mcp_buffer_bind(mcp_buffer_t* buffer, mcp_stream_t stream) {
    buffer->stream = stream;
}

/**
 * @brief allocate a buffer
 *
 * @param buffer pointer to the buffer
 * @param size   buffer size
 * 
 * @warning buffer should be deallocated with mcp_buffer_free after usage
 */
static inline void mcp_buffer_allocate(mcp_buffer_t* buffer, size_t size) {
    buffer->data = malloc(size);
    buffer->size = size;
    buffer->index = 0;
}

/**
 * @brief assign already allocated data to a buffer
 *
 * @param buffer pointer to the buffer
 * @param data   data value
 * @param size   data size
 */
static inline void mcp_buffer_set(mcp_buffer_t* buffer, char* data, size_t size) {
    buffer->data = data;
    buffer->size = size;
}

/**
 * @brief initialize a buffer with data from previously bound stream
 * 
 * @param buffer pointer to the buffer
 */
static inline void mcp_buffer_init(mcp_buffer_t* buffer) {
    mcp_stream_read(buffer->stream, buffer->data, buffer->size);
}

/**
 * @brief free a buffer
 *
 * @param buffer pointer to the buffer
 */
static inline void mcp_buffer_free(mcp_buffer_t* buffer) {
    free(buffer->data);
}

/**
 * @brief flush a buffer into previously bound stream
 * 
 * @param buffer the buffer
 */
static inline void mcp_buffer_flush(mcp_buffer_t* buffer) {
    mcp_stream_write(buffer->stream, buffer->data, buffer->size);
}

#endif /* MCP_IO_BUFFER_H */