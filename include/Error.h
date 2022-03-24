#include "Structs.h"

#ifndef ERROR_H
#define ERROR_H

/**
 * @brief 
 *   Send an error packet with a standard error message to a remote machine
 * @param fd
 *   A valid file descriptor
 * @param[in] address
 *   An address of the remote machine to send to
 * @param ec 
 *   A TFTP error code to send
 * @return void
 */
void StandardOutError(int, const Address*, int);

/**
 * @brief 
 *   Send an error packet with a custom error message to a remote machine
 * @param fd
 *   A valid file descriptor
 * @param[in] address
 *   An address of the remote machine to send to
 * @param str 
 *   Custom string message
 * @return void
 */
void CustomOutError(int, const Address*, const char*);

#endif