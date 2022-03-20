#include "Structs.h"
#include <stdio.h>

#ifndef IO_H
#define IO_H

/**
 * @brief 
 *   Write data from `DataPacket` to a file
 * @param file
 *   A pointer to the open file
 * @param packet 
 *   A pointer to `DataPacket` object to write from
 * @return int_fast32_t 
 *   Number of bytes written
 */
int_fast32_t DeserializePacket(FILE*, DataPacket*);

/**
 * @brief 
 *   Read data from a file to a `DataPacket`
 * @param file
 *   A pointer to the open file
 * @param packet 
 *   A pointer to `DataPacket` object to read to
 * @return int_fast32_t 
 *   Number of bytes read
 */
int_fast32_t SerializePacket(FILE*, DataPacket*);

#endif
