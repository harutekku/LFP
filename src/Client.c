// My headers
#include "../include/Client.h"
#include "../include/Protocol.h"
#include "../include/Utility.h"
#include "../include/Error.h"
#include "../include/IO.h"
// stdlib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Unix
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

static int_fast32_t HandleClientRead(const int32_t fd, FILE* file, Address* address) {
    //-------------------------------------------------------------------
    // Variables
    DataPacket data = { .Block = 0, .Size = 512 };
    AckPacket  ack;

    //-------------------------------------------------------------------
    do {
        // Set the size to the default array size
        data.Size = 512;

        // Receive data - updates `DataPacket::Size` field
        ReceiveData(fd, address, &data);

        // Save received bytes to the file
        DeserializePacket(file, &data);

        // Set the curret block to be acknowledged to the received block
        ack.Block = data.Block;

        // Acknowledge current block
        SendACK(fd, address, &ack);

    // Each DATA block should be 512 bytes wide
    } while (data.Size == 512);

    // We're done!
    fclose(file);
    return 0;
}

static int_fast32_t HandleClientSend(const int32_t fd, FILE* file, Address* address) {

    return 0;
}

int_fast32_t RunClient(ProgramArgs* pargs) {
    //-------------------------------------------------------------------
    // Variables
    int32_t   fd;
    Address   address;
    FILE*     file     = NULL;
    ReqPacket req      = { 
        .Opcode = pargs->InputFile? htons(WRQ) : htons(RRQ), 
        .ProgramMode = pargs->ProgramMode? pargs->ProgramMode : "octet",
        .ResName = (char*)pargs->ResName
    };

    //-------------------------------------------------------------------
    // First get a valid file descriptor
    if ((fd = GetClientSocket(pargs->TargetIP, pargs->RunIP6, &address)) == -1)
        return EXIT_FAILURE;

    // Send the first packet
    if (SendRequest(fd, &address, &req) == -1) {
        FreeAddress(&address);
        close(fd);
        return EXIT_FAILURE;
    }

    if (!(file = GetFilePointer(req.ResName, pargs->InputFile, req.ProgramMode[0] == 'o'))) {
        ERROR("Error opening file: %s", req.ResName);
        FreeAddress(&address);
        close(fd);
        return EXIT_FAILURE;
    }

    if (pargs->InputFile)
        HandleClientSend(fd, file, &address);
    else
        HandleClientRead(fd, file, &address);

    FreeAddress(&address);
    close(fd);

    return EXIT_SUCCESS;
}

int_fast32_t GetClientSocket(const char* address, bool useIP6, Address* retVal) {
    int32_t fd;
    int32_t ec;
    struct addrinfo hints;
    struct addrinfo* res;
    struct addrinfo* i;
    
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = useIP6? AF_INET6 : AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if ((ec = getaddrinfo(address, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "Error: %s\n", gai_strerror(ec));
        return -1;
    }

    for (i = res; i != NULL; i = i->ai_next) {
        if ((fd = socket(i->ai_family, i->ai_socktype, i->ai_protocol)) == -1) {
            perror("Error");
            continue;
        } 

        retVal->TheirAddressSize = i->ai_addrlen;
        retVal->TheirAddress = malloc(retVal->TheirAddressSize);
        memcpy(retVal->TheirAddress, i->ai_addr, retVal->TheirAddressSize);

        break;
    }

    freeaddrinfo(res);

    if (!i) {
        free(retVal->TheirAddress);
        return -1;
    } else
        return fd;
}

int_fast32_t SendRequest(const int32_t fd, Address* address, ReqPacket* payload) {
    char buffer[BUFFER_SIZE];
    size_t index = 0;
    size_t resLen = strlen(payload->ResName) + 1;
    size_t modeLen = strlen(payload->ProgramMode) + 1;

    SeqMemcpyDest(buffer, &payload->Opcode, sizeof(uint16_t), &index);
    SeqMemcpyDest(buffer, payload->ResName, resLen, &index);
    SeqMemcpyDest(buffer, payload->ProgramMode, modeLen, &index);

    if (sendto(fd, buffer, index, 0, address->TheirAddress, address->TheirAddressSize) <= 0)
        return -1;

    return 0;
}