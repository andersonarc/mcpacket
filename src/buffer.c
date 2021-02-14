/**
 * @file stream.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief buffered io
 * @version 0.1
 * @date 2021-02-14
 */
    /* includes */
#include "mcp/buffer.h" /* this */
#include "memory.h"     /* memory functions */

/**
 * @brief initialize a buffer
 *
 * @param buffer pointer to the buffer
 * @param size   buffer size
 */
MALLOC void buffer_init(buffer_t* buffer, size_t size) {
    buffer->data = malloc(sizeof(char) * size);
    buffer->size = size;
    buffer->index = 0;
}

/**
 * @brief deinitialize a buffer
 *
 * @param buffer pointer to the buffer
 */
void buffer_deinit(buffer_t* buffer) {
    free(buffer->data);
}

/**
 * @brief write to a buffer
 * 
 * @param buffer the buffer
 * @param src    data source
 * @param count  number of bytes to write
 */
void buffer_write(buffer_t buffer, void* src, size_t count) {
    memcpy(&buffer.data[buffer.index], src, count);
    buffer.index += count;
}

/**
 * @brief read from a buffer
 * 
 * @param buffer the buffer
 * @param dest   data destination
 * @param count  number of bytes to write
 */
void buffer_read(buffer_t buffer, void* dest, size_t count) {
    memcpy(dest, &buffer.data[buffer.index], count);
    buffer.index += count;
}

/**
 * @brief write a buffer into a stream
 * 
 * @param buffer the buffer
 * @param dest   stream destination
 */
void buffer_to_stream(buffer_t buffer, stream_t stream) {
    stream_write(stream, buffer.data, buffer.size);
}

/**
 * @brief read a stream into a buffer
 * 
 * @param buffer the buffer
 * @param src    stream source
 */
void stream_to_buffer(stream_t stream, buffer_t buffer) {
    stream_read(stream, buffer.data, buffer.size);
}