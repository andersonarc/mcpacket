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

    /* typedefs */
/**
 * @brief stream data type (file descriptor)
 */
typedef int stream_t;

    /* defines */
/**
 * @brief initialize a stream
 * 
 * @param stream pointer to the stream
 */
#define stream_init(stream)

/**
 * @brief deinitialize a stream
 * 
 * @param stream pointer to the stream
 * 
 * @return 0 on success or -1
 */
#define stream_deinit(stream) close(stream)

/**
 * @brief write to a stream
 * 
 * @param stream the stream
 * @param src    data source
 * @param count  number of bytes to write
 * 
 * @return number of bytes written or -1
 */
#define stream_write(stream, src, count) write(stream, src, count)

/**
 * @brief read from a stream
 * 
 * @param stream the stream
 * @param dest   data destination
 * @param count  number of bytes to read
 * 
 * @return number of bytes read or -1
 */
#define stream_read(stream, dest, count) read(stream, dest, count)

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
#define stream_write_variable(stream, variable) stream_write(stream, &(variable), sizeof(variable))

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
#define stream_read_variable(stream, variable) stream_read(stream, &(variable), sizeof(variable))

#endif /* MCP_STREAM_H */