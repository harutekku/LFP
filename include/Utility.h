#include "Structs.h"
#include <stdio.h>
#include <time.h>
#include <sys/socket.h>

#ifndef UTILITY_H
#define UTILITY_H

#define LOG_LEVEL 2

#if LOG_LEVEL > 0
#define ERROR(fmt, ...) do {                                                                        \
    time_t ERROR__TIME = time(NULL);                                                                \
    char ERROR__BUFFER[9] = { 0 };                                                                  \
    strftime(ERROR__BUFFER, 9, "%T", localtime(&ERROR__TIME));                                      \
    fprintf(stderr, "[ERROR @ %s in %s:%u]: " fmt, ERROR__BUFFER, __func__, __LINE__, __VA_ARGS__); \
} while (0)
#else
#define ERROR(fmt, ...)
#endif

#if LOG_LEVEL > 1
#define LOG(fmt, ...) do {                                                  \
    time_t LOG__TIME = time(NULL);                                          \
    char LOG__BUFFER[9] = { 0 };                                            \
    strftime(LOG__BUFFER, 9, "%T", localtime(&LOG__TIME));                  \
    printf("[LOG @ %s]: " fmt, LOG__BUFFER, __VA_ARGS__);                   \
} while (0)
#else
#define LOG(fmt, ...)
#endif

/**
 * @brief 
 *   Get a pointer to the open file
 * @param  filename
 *   A name of the file to open
 * @param forReading
 *   Whether or not to open file for reading
 * @param
 *   Whether or not to open file in binary mode
 * @return FILE* 
 *   A valid pointer to the open file, NULL on error
 */
FILE* GetFilePointer(const char*, bool, bool);

/**
 * @brief 
 *   Sequentially copy data chunks of size `size` to `dest`
 * @param dest
 *   Copy destination
 * @param[in] src 
 *   Copy source
 * @param size 
 *   Size of the single chunk
 * @param index 
 *   Offset
 * @note
 *   The effective address of the copy is calculated by 
 * `dest + *index` formula
 */
void SeqMemcpyDest(void* dest, const void* src, size_t size, size_t* index);

/**
 * @brief 
 *   Copy sequential data chunks of size `size` to `dest`
 * @param dest
 *   Copy destination
 * @param[in] src 
 *   Copy source
 * @param size 
 *   Size of the single chunk
 * @param index 
 *   Offset
 * @note
 *   The effective address to copy from is calculated by 
 * `src + *index` formula
 */
void SeqMemcpySrc(void* dest, const void* src, size_t size, size_t* index);

/**
 * @brief 
 *   Send raw data to address
 * @return ssize_t 
 */
ssize_t SendTo(int, const void*, size_t, int, const Address*);

/**
 * @brief 
 *   Receive raw data from address
 * @return ssize_t 
 */
ssize_t ReceiveFrom(int, void*, size_t, int, Address*);

#endif