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
 * @param[out] pargs
 *   Structure to hold parsed values
 * @return int
 *   Returns -1 on failure, 0 on success
 */
int ParseArgs(const int, const char*[], ProgramArgs*);

#endif