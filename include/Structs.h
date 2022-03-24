#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <netdb.h>

#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct ProgramArgs {
    const char*             TargetIP;
    const char*             ResName;
    const char*             ProgramMode;
    bool                    TreatAsInput;
    bool                    RunIP6;
} ProgramArgs;

typedef struct ReqPacket {
    uint16_t                Opcode;
    char*                   ResName;
    const char*             ProgramMode;
} ReqPacket;

typedef struct DataPacket {
    uint32_t                Block;
    size_t                  Size;
    char                    Data[512];
} DataPacket;

typedef struct AckPacket {
    uint32_t                Block;
} AckPacket;

typedef struct ErrPacket {
    uint16_t                EC;
    size_t                  Size;
    char                    Msg[64];
} ErrPacket;

typedef struct Address {
    struct sockaddr_storage RemoteAddress; // Don't want to malloc this everytime, plus there are only two instances of this structure anyway
    socklen_t               Size;
} Address;

#endif