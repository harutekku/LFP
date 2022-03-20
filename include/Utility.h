#include "Structs.h"
#include <stdio.h>
#include <time.h>

#ifndef UTILITY_H
#define UTILITY_H

#define LOG_LEVEL 2

#if LOG_LEVEL > 0
#define ERROR(fmt, ...) do {                                           \
    time_t ERROR__TIME = time(NULL);                                   \
    char ERROR__BUFFER[9] = { 0 };                                     \
    strftime(ERROR__BUFFER, 9, "%T", localtime(&ERROR__TIME));         \
    fprintf(stderr, "[ERROR @ %s]: " fmt, ERROR__BUFFER, __VA_ARGS__); \
} while (0)
#else
#define ERROR(fmt, ...)
#endif

#if LOG_LEVEL > 1
#define INFO(fmt, ...) do {                                                  \
    time_t INFO__TIME = time(NULL);                                          \
    char INFO__BUFFER[9] = { 0 };                                            \
    strftime(INFO__BUFFER, 9, "%T", localtime(&INFO__TIME));                 \
    printf("[LOG @ %s in %s()]: " fmt, INFO__BUFFER, __func__, __VA_ARGS__); \
} while (0)
#else
#define INFO(fmt, ...)
#endif

#if LOG_LEVEL > 2
#define DEBUG(fmt, ...) do {                                                                              \
    time_t DEBUG__TIME = time(NULL);                                                                      \
    char DEBUG__BUFFER[9] = { 0 };                                                                        \
    strftime(DEBUG__BUFFER, 9, "%T", localtime(&DEBUG__TIME));                                            \
    printf("[DEBUG @ %s -> %s:%s():%d]: " fmt, DEBUG__BUFFER, __FILE__, __func__, __LINE__, __VA_ARGS__); \
} while (0)
#else
#define DEBUG(fmt, ...)
#endif

/**
 * @brief 
 *   `free()` `Address->TheirAddress`
 * @param address
 *   A pointer to the `Address` struct with `malloc()`ated address to free
 * @return void
 */
void FreeAddress(Address*);

/**
 * @brief 
 *   `free()` `Address->TheirAddress` and `ReqPacket->Data`
 * @param req
 *   A pointer to the `ReqPacket` struct with `malloc()`ated data to free
 * @return void
 */
void FreeReq(ReqPacket*);

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
 * @param src 
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
 * @param src 
 *   Copy source
 * @param size 
 *   Size of the single chunk
 * @param index 
 *   Offset
 * @note
 *   The effective address of the copy is calculated by 
 * `src + *index` formula
 */
void SeqMemcpySrc(void* dest, const void* src, size_t size, size_t* index);

#endif