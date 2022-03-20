// My headers
#include "../include/Protocol.h"
#include "../include/Utility.h"
// stdlib
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
// Unix
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

int_fast32_t PeekOpcode(const int32_t fd, Address* address) {
    int16_t opcode;

    if (recvfrom(fd, &opcode, sizeof(int16_t), MSG_PEEK, address->TheirAddress, &address->TheirAddressSize) <= 0)
        return INV;
    else
        opcode = ntohs(opcode);
    
    if (opcode < 0 || opcode > 5)
        return INV;
    else
        return opcode;
}

int_fast32_t SendData(const int32_t fd, Address* address, DataPacket* packet) {
    char buffer[BUFFER_SIZE];
    const size_t size         = sizeof(packet->Block) + packet->Size + sizeof(uint16_t);
    const uint16_t opcode     = htons(DATA);
    const uint32_t block      = htonl(packet->Block);
    size_t index              = 0;

    SeqMemcpyDest(buffer, &opcode, sizeof(uint16_t), &index);
    SeqMemcpyDest(buffer, &block, sizeof(uint32_t), &index);
    SeqMemcpyDest(buffer, packet->Data, packet->Size, &index);

    if (sendto(fd, buffer, size, 0, address->TheirAddress, address->TheirAddressSize) <= 0) {
        return -1;
    }
    
    INFO("Sent data: %u:%u:%zu\n", DATA, packet->Block, packet->Size);

    return 0;
}

int_fast32_t SendACK(const int32_t fd, Address* address, AckPacket* packet) {
    char buffer[6];
    const uint16_t opcode = htons(ACK);
    const uint32_t block  = htonl(packet->Block);
    size_t index          = 0;

    SeqMemcpyDest(buffer, &opcode, sizeof(uint16_t), &index);
    SeqMemcpyDest(buffer, &block, sizeof(uint32_t), &index);

    if (sendto(fd, buffer, 6, 0, address->TheirAddress, address->TheirAddressSize) <= 0) {
        return -1;
    }

    INFO("Sent ack: %u:%u\n", ACK, packet->Block);

    return 0;
}

int_fast32_t SendError(const int32_t fd, Address* address, ErrPacket* packet) {
    char  buffer[BUFFER_SIZE];
    const size_t size     = 2 * sizeof(uint16_t) + packet->Size;
    const uint16_t opcode = htons(ERR);
    const uint16_t ec     = htons(packet->EC);
    size_t index          = 0;

    SeqMemcpyDest(buffer, &opcode, sizeof(uint16_t), &index);
    SeqMemcpyDest(buffer, &ec, sizeof(uint16_t), &index);
    SeqMemcpyDest(buffer, packet->Msg, packet->Size, &index);

    if (sendto(fd, buffer, size, 0, address->TheirAddress, address->TheirAddressSize) <= 0) {
        return -1;
    }

    INFO("Sent error: %u:%u\n", ERR, packet->EC);

    return 0;
}

int_fast32_t ReceiveData(const int32_t fd, Address* address, DataPacket* packet) {
    char buffer[BUFFER_SIZE];
    size_t index       = sizeof(uint16_t);
    int_fast32_t bytes = 0;

    if ((bytes = recvfrom(fd, buffer, BUFFER_SIZE, 0, address->TheirAddress, &address->TheirAddressSize)) <= 0) {
        return -1;
    }

    SeqMemcpySrc(&packet->Block, buffer, sizeof(uint32_t), &index);
    packet->Size = bytes - index;
    SeqMemcpySrc(packet->Data, buffer, packet->Size, &index);

    packet->Block = ntohl(packet->Block);

    INFO("Received data: %u:%u\n", DATA, packet->Block);

    return 0;
}

int_fast32_t ReceiveACK(const int32_t fd, Address* address, AckPacket* packet) {
    char buffer[6];
    size_t index = sizeof(uint16_t);

    if (recvfrom(fd, buffer, 6, 0, address->TheirAddress, &address->TheirAddressSize) <= 0) {
        return -1;
    }

    SeqMemcpySrc(&packet->Block, buffer, sizeof(uint32_t), &index);

    packet->Block = ntohl(packet->Block);

    INFO("Received ack: %u:%u\n", ACK, packet->Block);

    return 0;
}

int_fast32_t ReceiveError(const int32_t fd, Address* address, ErrPacket* packet) {
    char buffer[BUFFER_SIZE];
    size_t index = sizeof(uint16_t);

    if (recvfrom(fd, buffer, BUFFER_SIZE, 0, address->TheirAddress, &address->TheirAddressSize) <= 0) {
        return -1;
    }

    SeqMemcpySrc(&packet->EC, buffer, sizeof(uint16_t), &index);
    SeqMemcpySrc(&packet->Msg, buffer, packet->Size, &index);
    
    packet->EC = ntohs(packet->EC);

    INFO("Sent error: %u:%u\n", ERR, packet->EC);

    return 0;
}
