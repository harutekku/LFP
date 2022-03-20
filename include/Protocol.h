#include "Structs.h"

#ifndef PROTOCOL_H
#define PROTOCOL_H

#define PORT "42069"
#define BUFFER_SIZE 1024

#define INV  -1
#define RRQ   1
#define WRQ   2
#define DATA  3
#define ACK   4
#define ERR   5

/**
 * @brief 
 *   Peek the next opcode
 * @param fd
 *   A valid file descriptor
 * @param address 
 *   An address to read from
 * @return int_fast32_t 
 *   A valid opcode or `INV` on error
 */
int_fast32_t PeekOpcode(const int32_t, Address*);

/**
 * @brief 
 *   Send the data packet
 * @param fd
 *   A valid file descriptor
 * @param address 
 *   An address to read from
 * @param packet
 *   A valid DATA packet to send
 * @return int_fast32_t 
 *   0 on success, -1 on error
 */
int_fast32_t SendData(const int32_t, Address*, DataPacket*);

/**
 * @brief 
 *   Send the acknowledgment packet
 * @param fd
 *   A valid file descriptor
 * @param address 
 *   An address to read from
 * @param packet
 *   A valid ACK packet to send
 * @return int_fast32_t 
 *   0 on success, -1 on error
 */
int_fast32_t SendACK(const int32_t, Address*, AckPacket*);

/**
 * @brief 
 *   Send the error packet
 * @param fd
 *   A valid file descriptor
 * @param address 
 *   An address to read from
 * @param packet
 *   A valid ERROR packet to send
 * @return int_fast32_t 
 *   0 on success, -1 on error
 */
int_fast32_t SendError(const int32_t, Address*, ErrPacket*);

/**
 * @brief 
 *   Read the data packet
 * @param fd
 *   A valid file descriptor
 * @param address 
 *   An address to read from
 * @param packet
 *   A valid DATA packet to read to
 * @return int_fast32_t 
 *   0 on success, -1 on error
 * @note
 *   Stores the number of read bytes in `packet->Size` field
 */
int_fast32_t ReceiveData(const int32_t, Address*, DataPacket*);

/**
 * @brief 
 *   Receive the acknowledgment packet
 * @param fd
 *   A valid file descriptor
 * @param address 
 *   An address to read from
 * @param packet
 *   A valid ACK packet to read to
 * @return int_fast32_t 
 *   0 on success, -1 on error
 */
int_fast32_t ReceiveACK(const int32_t, Address*, AckPacket*);

/**
 * @brief 
 *   Receive the error packet
 * @param fd
 *   A valid file descriptor
 * @param address 
 *   An address to read from
 * @param packet
 *   A valid ERR packet to read to
 * @return int_fast32_t 
 *   0 on success, -1 on error
 * @note 
 *   The ERR packet needs to have dynamically allocated buffer
 */
int_fast32_t ReceiveError(const int32_t, Address*, ErrPacket*);

#endif