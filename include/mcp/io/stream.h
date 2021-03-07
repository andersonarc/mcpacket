/**
 * @file stream.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief stream io
 * @version 0.2
 * @date 2021-01-10
 */
    /* header guard */
#ifndef MCP_IO_STREAM_H
#define MCP_IO_STREAM_H

    /* includes */
#include <unistd.h> /* socket io */

    /* typedefs */
/**
 * @brief stream data type (file descriptor)
 */
typedef int mcp_stream_t;

    /* functions */
/**
 * @brief write to a stream
 * 
 * @param stream the stream
 * @param src    data source
 * @param count  number of bytes to write
 */
void mcp_stream_write(mcp_stream_t stream, char* src, size_t count);

/**
 * @brief read from a stream
 * 
 * @param stream the stream
 * @param dest   data destination
 * @param count  number of bytes to read
 */
void mcp_stream_read(mcp_stream_t stream, char* dest, size_t count);

#endif /* MCP_IO_STREAM_H */