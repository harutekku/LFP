#include "Structs.h"

#ifndef SERVER_H
#define SERVER_H

/**
 * @brief 
 *   Run the server program
 * @param[in] pargs
 *   Parsed program arguments
 * @return int
 *   EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
int RunServer(const ProgramArgs*);

/**
 * @brief 
 *   Get the socket file descriptor for server
 * @param port
 *   A port to bind to
 * @param useIP6 
 *   Whether or not to use IPv6
 * @return int
 *   Return a valid socket file descriptor on success, or -1 on error
 */
int GetServerSocket(const char*, bool);

/**
 * @brief 
 *   Receive initial request from client
 * @param fd
 *   A valid file descriptor
 * @param[out] address
 *   An address to read from
 * @param[out] payload
 *   A packet to read to
 * @return int
 *   0 on success, -1 on failure
 */
int ReceiveRequest(int, Address*, ReqPacket*);

#endif
