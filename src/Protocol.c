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

static int CheckIfError(uint16_t expectedOpcode, const char* buffer, size_t* index) {
    //-------------------------------------------------------------------
    // Variables
    uint16_t  opcode;
    ErrPacket err = { .Size = 64 };

    //-------------------------------------------------------------------
    // memcpy data
    SeqMemcpyDest(&opcode, buffer, sizeof(uint16_t), index);

    //-------------------------------------------------------------------
    // Deserialize
    opcode = ntohs(opcode);

    //-------------------------------------------------------------------
    // Check errors
    if (opcode == OPERROR) {
        ReadAsError(buffer, index, &err);
        ERROR("%s\n", err.Msg);
        return -1;
    } else if (opcode != expectedOpcode) {
        ERROR("Invalid opcode: expected (4), got (%u)\n", opcode);
        return -1;
    } else
        return 0;
}

ssize_t SendData(int fd, const Address* address, const DataPacket* packet) {
    //-------------------------------------------------------------------
    // Variables
    char            buffer[DATA_SIZE];
    const  size_t   size   = sizeof(packet->Block) + packet->Size + sizeof(uint16_t);
    const  uint16_t opcode = htons(OPDATA);
    const  uint32_t block  = htonl(packet->Block);
    size_t          index  = 0;

    //-------------------------------------------------------------------
    // memcpy data
    SeqMemcpyDest(buffer, &opcode, sizeof(uint16_t), &index);
    SeqMemcpyDest(buffer, &block, sizeof(uint32_t), &index);
    SeqMemcpyDest(buffer, packet->Data, packet->Size, &index);

    //-------------------------------------------------------------------
    // Send
    if (SendTo(fd, buffer, size, 0, address) == -1)
        return -1;
    
    LOG("Sent data: %u:%u:%zu\n", OPDATA, packet->Block, packet->Size);

    return index;
}

ssize_t SendACK(int fd, const Address* address, const AckPacket* packet) {
    //-------------------------------------------------------------------
    // Variables
    char           buffer[MIN_BUFFER_SIZE];
    const uint16_t opcode = htons(OPACK);
    const uint32_t block  = htonl(packet->Block);
    size_t         index  = 0;

    //-------------------------------------------------------------------
    // memcpy data
    SeqMemcpyDest(buffer, &opcode, sizeof(uint16_t), &index);
    SeqMemcpyDest(buffer, &block, sizeof(uint32_t), &index);

    //-------------------------------------------------------------------
    // Send
    if (SendTo(fd, buffer, ACK_SIZE, 0, address) == -1)
        return -1;

    LOG("Sent ack: %u:%u\n", OPACK, packet->Block);

    return index;
}

ssize_t SendError(int fd, const Address* address, const ErrPacket* packet) {
    //-------------------------------------------------------------------
    // Variables
    char           buffer[ERR_SIZE];
    const size_t   size   = 2 * sizeof(uint16_t) + packet->Size;
    const uint16_t opcode = htons(OPERROR);
    const uint16_t ec     = htons(packet->EC);
    size_t         index  = 0;

    //-------------------------------------------------------------------
    // memcpy data
    SeqMemcpyDest(buffer, &opcode, sizeof(uint16_t), &index);
    SeqMemcpyDest(buffer, &ec, sizeof(uint16_t), &index);
    SeqMemcpyDest(buffer, packet->Msg, packet->Size, &index);

    //-------------------------------------------------------------------
    // Send
    if (SendTo(fd, buffer, size, 0, address) == -1)
        return -1;

    LOG("Sent error: %u:%u\n", OPERROR, packet->EC);

    return index;
}

ssize_t ReceiveData(int fd, Address* address, DataPacket* packet) {
    //-------------------------------------------------------------------
    // Variables
    char      buffer[DATA_SIZE];
    size_t    index  = 0;
    ssize_t   bytes  = 0;

    //-------------------------------------------------------------------
    // Init buffer
    memset(buffer, 0, MIN_BUFFER_SIZE);

    //-------------------------------------------------------------------
    // Check errors
    if ((bytes = ReceiveFrom(fd, buffer, DATA_SIZE, 0, address)) <= 0)
        return bytes;

    if (CheckIfError(OPDATA, buffer, &index))
        return -1;

    //-------------------------------------------------------------------
    // memcpy data
    SeqMemcpySrc(&packet->Block, buffer, sizeof(uint32_t), &index);
    packet->Size = bytes - index;
    SeqMemcpySrc(packet->Data, buffer, packet->Size, &index);

    //-------------------------------------------------------------------
    // Deserialize
    packet->Block = ntohl(packet->Block);

    LOG("Received data: %u:%u:%zu\n", OPDATA, packet->Block, packet->Size);

    return bytes;
}

ssize_t ReceiveACK(int fd, Address* address, AckPacket* packet) {
    //-------------------------------------------------------------------
    // Variables
    char      buffer[MIN_BUFFER_SIZE];
    size_t    index = 0;
    ssize_t   bytes;

    //-------------------------------------------------------------------
    // Init buffer
    memset(buffer, 0, MIN_BUFFER_SIZE);

    //-------------------------------------------------------------------
    // Check errors
    if ((bytes = ReceiveFrom(fd, buffer, MIN_BUFFER_SIZE, 0, address)) <= 0)
        return bytes;

    if (CheckIfError(OPACK, buffer, &index))
        return -1;

    //-------------------------------------------------------------------
    // memcpy data
    SeqMemcpySrc(&packet->Block, buffer, sizeof(uint32_t), &index);

    //-------------------------------------------------------------------
    // Deserialize
    packet->Block = ntohl(packet->Block);

    LOG("Received ack: %u:%u\n", OPACK, packet->Block);

    return bytes;
}

void ReadAsError(const char* buffer, size_t* index, ErrPacket* packet) {
    //-------------------------------------------------------------------
    // memcpy data
    SeqMemcpySrc(&packet->EC,  buffer, sizeof(uint16_t), index);
    SeqMemcpySrc(&packet->Msg, buffer, packet->Size, index);

    //-------------------------------------------------------------------
    // Deserialize
    packet->EC = ntohs(packet->EC);
    packet->Msg[packet->Size - 1] = '\0';

    LOG("Received error: %u:%u:%s\n", OPERROR, packet->EC, packet->Msg);
}
