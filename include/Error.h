#include "Structs.h"

#ifndef ERROR_H
#define ERROR_H

/**
 * @brief 
 *   Send an error packet to a remote machine
 * @param fd
 *   A valid file descriptor
 * @param address
 *   Address of the remote machine to send to
 * @param ec 
 *   Error code to send
 * @return
 *   Nothing
 */
void HandleError(const int32_t, Address*, uint8_t);

#endif