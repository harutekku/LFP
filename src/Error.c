// My headers
#include "../include/Error.h"
#include "../include/Protocol.h"
// stdlib
#include <string.h>

void HandleError(const int32_t fd, Address* address, uint8_t ec) {
    ErrPacket packet = { 0 };

    switch (ec) {
    case 0:
        packet.EC = 0;
        packet.Msg = "Undefined mode";
        packet.Size = strlen(packet.Msg);
        break;
    case 1:
        packet.EC = 1;
        packet.Msg = "File not found";
        packet.Size = strlen(packet.Msg);
        break;
    case 2:
        packet.EC = 2;
        packet.Msg = "Access violation";
        packet.Size = strlen(packet.Msg);
        break;
    case 3:
        packet.EC = 3;
        packet.Msg = "Disk full or allocation exceeded";
        packet.Size = strlen(packet.Msg);
        break;
    case 4:
        packet.EC = 5;
        packet.Msg = "Illegal FTFP operation";
        packet.Size = strlen(packet.Msg);
        break;
    case 5:
        packet.EC = 5;
        packet.Msg = "Unknown transfer ID";
        packet.Size = strlen(packet.Msg);
        break;
    case 6:
        packet.EC = 6;
        packet.Msg = "File already exists";
        packet.Size = strlen(packet.Msg);
        break;
    case 7:
        packet.EC = 7;
        packet.Msg = "No such user";
        packet.Size = strlen(packet.Msg);
        break;
    default:
        return;
    }
    SendError(fd, address, &packet);
}