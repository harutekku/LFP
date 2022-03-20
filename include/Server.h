#include "Structs.h"

#ifndef SERVER_H
#define SERVER_H

/**
 * @brief 
 *   Run the server program
 * @param pargs
 *   Parsed program arguments
 * @return int_fast32_t 
 *   EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
int_fast32_t RunServer(const ProgramArgs*);

/**
 * @brief 
 *   Get the socket file descriptor for server
 * @param port
 *   A port to bind to
 * @param useIP6 
 *   Whether or not to use IPv6
 * @return int_fast32_t 
 *   Return a valid socket file descriptor on success, or -1 on error
 */
int_fast32_t GetServerSocket(const char*, bool);

/**
 * @brief 
 *   Receive initial request from client
 * @param fd
 *   A valid file descriptor
 * @param address
 *   An address to read from
 * @param payload
 *   A packet to read to
 * @return int_fast32_t 
 *   0 on success, -1 on failure
 */
int_fast32_t ReceiveRequest(const int32_t, Address*, ReqPacket*);

#endif
