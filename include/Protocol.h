#include "Structs.h"

#ifndef PROTOCOL_H
#define PROTOCOL_H

#define PORT "42069"

#define sizeofm(S, M) sizeof((((S*)NULL)->M))

#define DATA_SIZE       (sizeof(uint16_t) + sizeofm(DataPacket, Block) + sizeofm(DataPacket, Data))
#define ERR_SIZE        (sizeof(uint16_t) + sizeofm(ErrPacket, EC)     + sizeofm(ErrPacket, Msg))
#define ACK_SIZE        (sizeof(uint16_t) + sizeofm(AckPacket, Block))
#define MIN_BUFFER_SIZE ERR_SIZE
#define MAX_BUFFER_SIZE 1024

#define OPNVAL         -1
#define OPREAD          1
#define OPWRITE         2
#define OPDATA          3
#define OPACK           4
#define OPERROR         5
                    
#define ERCUSTOM        0
#define ERNFIND         1
#define ERACCESS        2
#define ERFULL          3
#define ERILLEGAL       4
#define ERNKNOWNID      5
#define ERFEXISTS       6
#define ERBADUSR        7

/**
 * @brief 
 *   Send the data packet
 * @param fd
 *   A valid file descriptor
 * @param[in] address 
 *   An address to send to
 * @param[in] packet
 *   A valid DATA packet to send
 * @return ssize_t
 *   Number of sent bytes, -1 on error
 */
ssize_t SendData(int, const Address*, const DataPacket*);

/**
 * @brief 
 *   Send the acknowledgment packet
 * @param fd
 *   A valid file descriptor
 * @param[in] address 
 *   An address to send to
 * @param[in] packet
 *   A valid ACK packet to send
 * @return ssize_t
 *   Number of sent bytes, -1 on error
 */
ssize_t SendACK(int, const Address*, const AckPacket*);

/**
 * @brief 
 *   Send the error packet
 * @param fd
 *   A valid file descriptor
 * @param[in] address 
 *   An address to send to
 * @param[in] packet
 *   A valid ERROR packet to send
 * @return ssize_t
 *   Number of sent bytes, -1 on error
 */
ssize_t SendError(int, const Address*, const ErrPacket*);

/**
 * @brief 
 *   Read the data packet
 * @param fd
 *   A valid file descriptor
 * @param[out] address 
 *   An address to read from
 * @param[out] packet
 *   A valid DATA packet to read to
 * @return ssize_t
 *   Number of received bytes, -1 on error, 0 on timeout
 */
ssize_t ReceiveData(int, Address*, DataPacket*);

/**
 * @brief 
 *   Receive the acknowledgment packet
 * @param fd
 *   A valid file descriptor
 * @param[out] address 
 *   An address to read from
 * @param[out] packet
 *   A valid ACK packet to read to
 * @return ssize_t 
 *   Number of received bytes, -1 on error, 0 on timeout
 */
ssize_t ReceiveACK(int, Address*, AckPacket*);

/**
 * @brief 
 *   Read the error packet
 * @param buffer
 *   A buffer with the error packet data
 * @param[out] index 
 *   An index to start reading from
 * @param[out] packet
 *   A ErrPacket to read to
 * @return void
 */
void ReadAsError(const char*, size_t*, ErrPacket*);

#endif