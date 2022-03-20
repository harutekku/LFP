#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <netdb.h>

#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct ProgramArgs {
    const char* TargetIP;
    const char* ResName;
    const char* ProgramMode;
    const char* InputFile;
    bool        RunIP6;
} ProgramArgs;

typedef struct ReqPacket {
    uint16_t    Opcode;
    char*       ResName;
    const char* ProgramMode;
} ReqPacket;

typedef struct DataPacket {
    uint32_t    Block;
    size_t      Size;
    char        Data[512];
} DataPacket;

typedef struct AckPacket {
    uint32_t    Block;
} AckPacket;

typedef struct ErrPacket {
    uint16_t    EC;
    size_t      Size;
    char*       Msg;
} ErrPacket;

typedef struct Address {
    void*       TheirAddress;
    socklen_t   TheirAddressSize;
} Address;

#endif