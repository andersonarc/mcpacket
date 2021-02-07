/**
 * @file stream.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief abstract stream io
 * @version 0.1
 * @date 2021-01-10
 */
    /* header guard */
#ifndef MCP_STREAM_H
#define MCP_STREAM_H

    /* includes */
#include <unistd.h> /* actual io */

    /* defines */
/**
 * @brief stream data type (file descriptor)
 */
#define stream_t int

/**
 * @brief write to a stream
 * 
 * @param stream stream_t to write into
 * @param buffer buffer to read from
 * @param count  number of bytes to write
 * 
 * @return number of bytes written or -1
 */
#define stream_write(stream, buffer, count) write(stream, buffer, count)

/**
 * @brief read from a stream
 * 
 * @param stream stream_t to read from
 * @param buffer buffer to write into
 * @param count  number of bytes to read
 * 
 * @return number of bytes read or -1
 */
#define stream_read(stream, buffer, count) read(stream, buffer, count)

/**
 * @brief close a stream
 * 
 * @return 0 on success or -1
 */
#define stream_close(stream) close(stream)

/**
 * @brief write a variable to a stream
 * 
 * @param stream   stream_t to write into
 * @param variable variable to write from
 * 
 * @warning use with caution,
 *             inapproporiate usage can lead to unexpected behaviour
 * 
 * @return number of bytes written or -1
 */
#define stream_write_variable(stream, variable) write(stream, &(variable), sizeof(variable))

/**
 * @brief read a variable from a stream
 * 
 * @param stream   stream_t to read from
 * @param variable variable to read into
 * 
 * @warning use with caution,
 *             inapproporiate usage can lead to unexpected behaviour
 * 
 * @return number of bytes read or -1
 */
#define stream_read_variable(stream, variable) read(stream, &(variable), sizeof(variable))

#endif /* MCP_STREAM_H */