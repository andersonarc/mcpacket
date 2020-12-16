/**
 * @file translation_utils.c
 * @author andersonarc (andersonarc@github.com)
 * @brief utilities required for transition from C++ to C
 * @version 0.1
 * @date 2020-12-12
 * 
 * @copyright andersonarc
 */
    /* includes */
#include "translation_utils.h" /* this */

/**
 * @brief print message and exit with error
 */
void runtime_error(string_t message) {
    fputs(message, stdout);
    fputs(message, stderr);
    exit(EXIT_FAILURE);
}

/**
 * todo
 * @brief nbt stubs 
 */
void nbt_encode_full() {}
void nbt_decode_full() {}
void nbt_read_string() {}