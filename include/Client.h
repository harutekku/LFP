#include "Structs.h"

#ifndef CLIENT_H
#define CLIENT_H

/**
 * @brief 
 *   Run the client program
 * @param[in] pargs 
 *   Parsed program arguments
 * @return int
 *   EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
int RunClient(const ProgramArgs*);

/**
 * @brief 
 *   Get the socket file descriptor for client
 * @param address
 *   Alpha-numeric address of the target machine
 * @param useIP6 
 *   Whether or not use IPv6
 * @param[out] retVal
 *   A pointer to the `Address` structure that holds the address the host connected to
 * @return int
 *   Return a valid socket file descriptor on success, or -1 on error
 */
int GetClientSocket(const char*, bool, Address*);

/**
 * @brief 
 *   Send initial request to the remote machine
 * @param fd
 *   A valid file descriptor
 * @param[in] address 
 *   An address to send request to
 * @param[in] req
 *   A valid request
 * @return int
 *   0 on success, -1 on error
 */
int SendRequest(int, const Address*, const ReqPacket*);

#endif