#include "Structs.h"

#ifndef ARGS_H
#define ARGS_H

/**
 * @brief 
 *   Parse command line arguments
 * @param argc
 *   Count of command line arguments
 * @param argv
 *   Argument vector
 * @param pargs
 *   Structure to hold parsed values
 * @return int_fast32_t
 *   Returns -1 on failure, 0 on success
 */
int_fast32_t ParseArgs(const int32_t, const char*[], ProgramArgs*);

#endif