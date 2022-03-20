/**
 * @file 
 *   main.c
 * @author 
 *   Harutekku
 * @brief 
 *   Contains the main function
 * @version 
 *   0.1
 * @date 
 *   2022-03-17
 * @copyright 
 *   Copyright (c) 2022
 */
// My headers
#include "../include/Client.h"
#include "../include/Server.h"
#include "../include/Args.h"
// stdlib
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief 
 *   Program entry point
 * @param 
 *   argc Number of program arguments
 * @param 
 *   argv Arguments vector
 * @return 
 *   int Returns EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int main(const int argc, const char* argv[]) {
    ProgramArgs pargs = { 0 };
    
    if (ParseArgs(argc, argv, &pargs))
        return EXIT_FAILURE;

    if (pargs.TargetIP && pargs.ResName)
        return RunClient(&pargs);
    else 
        return RunServer(&pargs);
}