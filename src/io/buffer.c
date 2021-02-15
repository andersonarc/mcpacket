/**
 * @file buffer.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief buffered io stream
 * @version 0.3
 * @date 2021-02-14
 */
    /* includes */
#include "mcp/io/buffer.h" /* this */
#include <memory.h>        /* memory functions  */
#include <malloc.h>        /* memore allocation */

    /* functions */
/**
 * @brief bind a buffer to a stream
 *
 * @param buffer pointer to the buffer
 * @param size   buffer size
 */
void mcp_buffer_bind(mcp_buffer_t* buffer, mcp_stream_t stream) {
    buffer->stream = stream;
}

/**
 * @brief allocate a buffer
 *
 * @param buffer pointer to the buffer
 * @param size   buffer size
 */
MALLOC void mcp_buffer_allocate(mcp_buffer_t* buffer, size_t size) {
    buffer->data = malloc(sizeof(char) * size);
    buffer->size = size;
    buffer->index = 0;
}

/**
 * @brief initialize a buffer
 * 
 * @param buffer pointer to the buffer
 */
void mcp_buffer_init(mcp_buffer_t* buffer) {
    mcp_stream_read(buffer->stream, buffer->data, buffer->size);
}

/**
 * @brief free a buffer
 *
 * @param buffer pointer to the buffer
 * 
 * @warning it's the caller's responsibility to flush the buffer/close the stream
 */
void mcp_buffer_free(mcp_buffer_t* buffer) {
    free(buffer->data);
}

/**
 * @brief flush a buffer into bound stream
 * 
 * @param buffer the buffer
 */
void mcp_buffer_flush(mcp_buffer_t* buffer) {
    mcp_stream_write(buffer->stream, buffer->data, buffer->size);
}

/**
 * @brief write to a buffer
 * 
 * @param buffer pointer to the buffer
 * @param src    data source
 * @param count  number of bytes to write
 * 
 * @warning use with caution, segmentation fault is possible
 */
void mcp_buffer_write(mcp_buffer_t* buffer, void* src, size_t count) {
    memcpy(&(buffer->data[buffer->index]), src, count);
    buffer->index += count;
}

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
void mcp_buffer_read(mcp_buffer_t* buffer, void* dest, size_t count) {
    memcpy(dest, &(buffer->data[buffer->index]), count);
    buffer->index += count;
}