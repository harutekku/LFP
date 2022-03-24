// My headers
#include "../include/Args.h"
// stdlib
#include <stdio.h>
#include <string.h>
// Unix/GNU
#include <argp.h>

/**
 * @brief 
 *   Describes program version
 */
const char* argp_program_version = "version 0.1";

/**
 * @brief 
 *   Structure representing program arguments with 
 *   additional count of required ones 
 */
struct Arguments {
    ProgramArgs* Args;
    int Size;
};

/**
 * @brief 
 *    Program options
 */
static const struct argp_option options[] = {
    { "ip6",  1488,     NULL, 0, "Use IPv6 for connection", 0 },
    { "mode",  'm',   "MODE", 0, "Mode to run in",          0 },
    { "input", 'i',     NULL, 0, "Treat FILE as input",     0 },
    {                         0                               }
};

/**
 * @brief 
 *   Parse mode 
 * @param arg 
 * @param args 
 * @param state 
 */
static void ParseMode(const char* arg, struct Arguments* args, struct argp_state* state) {
    if (strcmp(arg, "octet") == 0)
        args->Args->ProgramMode = "octet";
    else if (strcmp(arg, "netascii") == 0)
        args->Args->ProgramMode = "netascii";
    else
        argp_failure(state, 1, 1, "Invalid mode");
}

static int ParseOptions(int key, char* arg, struct argp_state* state) {
    struct Arguments* args = state->input;
    switch (key) {
    case 1488:
        args->Args->RunIP6 = true;
        break;
    case 'm':
        ParseMode(arg, args, state);
        break;
    case 'i':
        args->Args->TreatAsInput = true;
        break;
    case ARGP_KEY_ARG:
        --args->Size;
        if (args->Size == 1)
            args->Args->TargetIP = arg;
        else if (args->Size == 0)
            args->Args->ResName = arg;
        break;
    case ARGP_KEY_END:
        if (args->Size > 0 && args->Size != 2)
            argp_failure(state, 1, 1, "Too few arguments");
        else if (args->Size < 0)
            argp_failure(state, 1, 1, "Too many arguments");
        break;
    }
    return 0;
}

int ParseArgs(const int argc, const char* argv[], ProgramArgs* pargs) {
    const struct argp argp = { options, ParseOptions, "IP FILE\n", "", NULL, NULL, NULL };
    struct Arguments  args = { pargs, 2 };
    return argp_parse(&argp, argc, (char**)argv, 0, 0, &args);
}
