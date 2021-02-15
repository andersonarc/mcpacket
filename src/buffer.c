/**
 * @file stream.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief buffered io
 * @version 0.1
 * @date 2021-02-14
 */
    /* includes */
#include "mcp/buffer.h" /* this */
#include <memory.h>     /* memory functions  */
#include <malloc.h>     /* memore allocation */

/**
 * @brief bind a buffer to a stream
 *
 * @param buffer pointer to the buffer
 * @param size   buffer size
 */
void buffer_bind(buffer_t* buffer, stream_t stream) {
    buffer->stream = stream;
}

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
    stream_read(buffer->stream, buffer->data, buffer->size);
}

/**
 * @brief deinitialize a buffer
 *
 * @param buffer pointer to the buffer
 * 
 * @warning it's the caller's responsibility to flush the buffer/close the stream
 */
void buffer_deinit(buffer_t* buffer) {
    free(buffer->data);
}

/**
 * @brief flush a buffer into bound stream
 * 
 * @param buffer the buffer
 */
void buffer_flush(buffer_t* buffer) {
    stream_write(buffer->stream, buffer->data, buffer->size);
}

/**
 * @brief write to a buffer
 * 
 * @param buffer pointer to the buffer
 * @param src    data source
 * @param count  number of bytes to write
 */
void buffer_write(buffer_t* buffer, void* src, size_t count) {
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
 * @todo
 * @warning buffer overflow is possible
 */
void buffer_read(buffer_t* buffer, void* dest, size_t count) {
    memcpy(dest, &(buffer->data[buffer->index]), count);
    buffer->index += count;
}