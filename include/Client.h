#include "Structs.h"

#ifndef CLIENT_H
#define CLIENT_H

/**
 * @brief 
 *   Run the client program
 * @param pargs 
 *   Parsed program arguments
 * @return int_fast32_t 
 *   EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
int_fast32_t RunClient(ProgramArgs*);

/**
 * @brief 
 *   Get the socket file descriptor for client
 * @param address
 *   Alpha-numeric address of the target machine
 * @param useIP6 
 *   Whether or not use IPv6
 * @param address
 *   A pointer to the `Address` structure that holds the address the host connected too
 * @return int_fast32_t 
 *   Return a valid socket file descriptor on success, or -1 on error
 */
int_fast32_t GetClientSocket(const char*, bool, Address*);

/**
 * @brief 
 *   Send initial request to the remote machine
 * @param fd
 *   A valid file descriptor
 * @param address 
 *   An address to send request to
 * @param req
 *   A valid request
 * @return int_fast32_t 
 */
int_fast32_t SendRequest(const int32_t, Address*, ReqPacket*);

#endif