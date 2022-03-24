#include "Structs.h"
#include <stdio.h>

#ifndef IO_H
#define IO_H

/**
 * @brief 
 *   Write data from `DataPacket` to a file
 * @param file
 *   A pointer to the open file
 * @param[in] packet 
 *   A pointer to `DataPacket` object to write from
 * @return size_t
 *   Number of bytes written
 */
size_t DeserializePacket(FILE*, const DataPacket*);

/**
 * @brief 
 *   Read data from a file to a `DataPacket`
 * @param file
 *   A pointer to the open file
 * @param[out] packet 
 *   A pointer to `DataPacket` object to read to
 * @return int
 *   Number of bytes read
 */
size_t SerializePacket(FILE*, DataPacket*);

#endif
