/**
 * @file stream.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief stream io
 * @version 0.4
 * @date 2021-03-07
 */
    /* includes */
#include "mcp/io/stream.h" /* this */
#include "csafe/assertd.h" /* debug assertions */

    /* functions */
/**
 * @brief write to a stream
 * 
 * @param stream the stream
 * @param src    data source
 * @param count  number of bytes to write
 */
void mcp_stream_write(mcp_stream_t stream, char* src, size_t count) {
    ssize_t written = write(stream, src, count);
    assertd_false_custom("mcp_stream_write", written < 0, "unable to write into a stream");
    if (written == count) {
        return;
    } else {
        while (written < count) {
            #ifdef NDEBUG
                written += write(stream, &src[written], count - written);
            #else
                ssize_t currently_written = write(stream, &src[written], count - written);
                assertd_false_custom("mcp_stream_write", currently_written < 0, "unable to write into a stream");
                written += currently_written;
            #endif /* NDEBUG */
        }
    }
}

/**
 * @brief read from a stream
 * 
 * @param stream the stream
 * @param dest   data destination
 * @param count  number of bytes to read
 */
void mcp_stream_read(mcp_stream_t stream, char* dest, size_t count) {
    ssize_t received = read(stream, dest, count);
    assertd_false_custom("mcp_stream_read", received < 0, "unable to read from a stream");
    if (received == count) {
        return;
    } else {
        while (received < count) {
            #ifdef NDEBUG
                received += read(stream, &dest[received], count - received);
            #else
                ssize_t currently_received = read(stream, &dest[received], count - received);
                assertd_false_custom("mcp_stream_write", currently_received < 0, "unable to read from a stream");
                received += currently_received;
            #endif /* NDEBUG */
        }
    }
}