/**
 * @file misc.c
 * @author andersonarc (andersonarc@github.com)
 * @brief miscelannous utilities
 * @version 0.4
 * @date 2020-12-12
 */
    /* includes */
#include "mcp/misc.h" /* this */

    /* functions */
/**
 * @brief print warning
 * 
 * @param message message string
 */
void runtime_warning(string_t message) {
    fputs(message, stdout);
    fputs(message, stderr); //todo use csafe in this library
}

/**
 * @brief print message and exit with error
 * 
 * @param message message string
 */
void runtime_error(string_t message) {
    fputs(message, stdout);
    fputs(message, stderr);
    exit(EXIT_FAILURE);
}
