// My headers
#include "../include/Error.h"
#include "../include/Protocol.h"
// stdlib
#include <string.h>

void StandardOutError(int fd, const Address* address, int ec) {
    //-------------------------------------------------------------------
    // Variables
    static const ErrPacket errors[] = {
        { ERNFIND,    14, "File not found"                   },
        { ERACCESS,   16, "Access violation"                 },
        { ERFULL,     32, "Disk full or allocation exceeded" },
        { ERILLEGAL,  22, "Illegal FTFP operation"           },
        { ERNKNOWNID, 19, "Unknown transfer ID"              },
        { ERFEXISTS,  19, "File already exists"              },
        { ERBADUSR,   12, "No such user"                     }
    };

    //-------------------------------------------------------------------
    if (ec > 0 && ec < 8)
        SendError(fd, address, &errors[ec - 1]);
}

void CustomOutError(int fd, const Address* address, const char* msg) {
    //-------------------------------------------------------------------
    // Variables
    const size_t size   = strlen(msg) + 1;
    ErrPacket    packet = { .EC = ERCUSTOM, .Size = size > 64? 64 : size };

    //-------------------------------------------------------------------
    // memcpy data
    memcpy(packet.Msg, msg, packet.Size);
    packet.Msg[packet.Size - 1] = '\0';

    // Send
    SendError(fd, address, &packet);
}