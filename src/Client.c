// My headers
#include "../include/Client.h"
#include "../include/Protocol.h"
#include "../include/Utility.h"
#include "../include/Error.h"
#include "../include/IO.h"
// stdlib
#include <asm-generic/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
// Unix
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
    
static int ReceiveInitialACK(int fd, Address* address, AckPacket* packet) {
    ssize_t status;

    status = ReceiveACK(fd, address, packet);

    if (status == -1 || status == 0)
        return -1;
    else if (packet->Block != 0)
        return -1;
    else
        return 0;
}

static int HandleClientRead(int fd, FILE* file, Address* address) {
    //-------------------------------------------------------------------
    // Variables
    ssize_t      status;
    int          retries = 5;
    DataPacket   data = { .Block = 0 };
    AckPacket    ack  = { .Block = 0 };

    //-------------------------------------------------------------------
    do {
        // Set the size to the default array size
        data.Size = 512;

        status = ReceiveData(fd, address, &data);
        if (status == 0) {
            // Timeout
            LOG("Timed out, retries left: %d...\n", retries);
            SendACK(fd, address, &ack);
            --retries;
            continue;
        } else if (status == -1) {
            // Socket error that we can't handle
            fclose(file);
            return -1;
        } else if (ack.Block == data.Block) {
            // Block mismatch - protocol error that we can handle
            LOG("Packet got lost, retries left: %d...\n", retries);
            SendACK(fd, address, &ack);
            --retries;
            continue;
        } else {
            // Got a good block
            ack.Block = data.Block;
            retries = 5;
        }
            
        //-------------------------------------------------------------------
        // Acknowledge current block
        SendACK(fd, address, &ack);

        // Save received bytes to the file
        DeserializePacket(file, &data);

    } while (data.Size == 512 && retries);

    // We're done!
    fclose(file);
    return 0;
}

static int HandleClientWrite(int fd, FILE* file, Address* address) {
    //-------------------------------------------------------------------
    // Variables
    ssize_t    status;
    int        retries;
    bool       next = true; // Used to mitigate SAS
    DataPacket data = { .Block = 0, .Size = 512 };
    AckPacket  ack;

    //-------------------------------------------------------------------
    if (ReceiveInitialACK(fd, address, &ack)) {
        ERROR("%s\n", "Request timed out failed");
        fclose(file);
        return -1;
    }

    do {
        //-------------------------------------------------------------------
        // Prepare packet to send
        data.Size = SerializePacket(file, &data);

        // Next block
        ++data.Block;

        for (retries = 5; retries != 0; --retries) {
            //-------------------------------------------------------------------
            // Send the next packet only if the previous one was acknowledged
            if (next && SendData(fd, address, &data) == -1) {
                ERROR("%s\n", "Failed to send data");
                CustomOutError(fd, address, "Internal server error");
                fclose(file);
                return -1;
            }

            //-------------------------------------------------------------------
            // Wait for acknowledgement
            status = ReceiveACK(fd, address, &ack);

            //-------------------------------------------------------------------
            // Check error
            if (status == 0)
                LOG("Timed out, retries left: %d...\n", (int)retries);
            else if (status == -1) {
                ERROR("%s\n", "Failed to receive data; system error");
                CustomOutError(fd, address, "Internal server error");
                fclose(file);
                return -1;
            } else if (ack.Block == (data.Block - 1)) {
                LOG("Received retransmitted ACK packet: got %u, expected %u, retries left: %d...\n", ack.Block, data.Block, (int)retries);
                next = false;
            } else {
                next = true;
                break;
            }
        }

    //-------------------------------------------------------------------    
        
        if (!retries) {
            ERROR("%s\n", "Max retries reached");
            CustomOutError(fd, address, "Max retries reached");
            fclose(file);
            return -1;
        }
    } while (data.Size == 512);

    fclose(file);
    return 0;
}

int RunClient(const ProgramArgs* pargs) {
    //-------------------------------------------------------------------
    // Variables
    int       fd;
    Address   address;
    FILE*     file     = NULL;
    ReqPacket req      = { 
        .Opcode        = pargs->TreatAsInput? htons(OPWRITE)     : htons(OPREAD), 
        .ProgramMode   = pargs->ProgramMode ? pargs->ProgramMode : "octet",
        .ResName       = (char*)pargs->ResName
    };

    //-------------------------------------------------------------------
    // First get a valid file descriptor
    if ((fd = GetClientSocket(pargs->TargetIP, pargs->RunIP6, &address)) == -1)
        return EXIT_FAILURE;

    // Send the first packet
    if (SendRequest(fd, &address, &req) == -1) {
        close(fd);
        return EXIT_FAILURE;
    }

    // Open the file
    if (!(file = GetFilePointer(req.ResName, pargs->TreatAsInput, req.ProgramMode[0] == 'o'))) {
        ERROR("Error opening file: %s\n", req.ResName);
        close(fd);
        return EXIT_FAILURE;
    }

    // If the ResName is supposed to be treated as input, send it
    if (pargs->TreatAsInput)
        HandleClientWrite(fd, file, &address);
    else
        HandleClientRead(fd, file, &address);

    // We're done!
    close(fd);

    return EXIT_SUCCESS;
}

int GetClientSocket(const char* address, bool useIP6, Address* retVal) {
    //-------------------------------------------------------------------
    // Variables
    int              fd;
    int              ec;
    struct addrinfo  hints;
    struct addrinfo* res;
    struct addrinfo* i;

    //-------------------------------------------------------------------
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family   = useIP6? AF_INET6 : AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags    = AI_PASSIVE;

    //-------------------------------------------------------------------
    if ((ec = getaddrinfo(address, PORT, &hints, &res)) != 0) {
        ERROR("%s\n", gai_strerror(ec));
        return -1;
    }

    //-------------------------------------------------------------------
    for (i = res; i != NULL; i = i->ai_next) {
        if ((fd = socket(i->ai_family, i->ai_socktype, i->ai_protocol)) == -1) {
            ERROR("socket: %s\n", gai_strerror(errno));
            continue;
        } 

        retVal->Size = i->ai_addrlen;
        memcpy(&retVal->RemoteAddress, i->ai_addr, retVal->Size);

        break;
    }

    freeaddrinfo(res);

    //-------------------------------------------------------------------
    if (!i) {
        return -1;
    } else
        return fd;
}

int SendRequest(const int fd, const Address* address, const ReqPacket* payload) {
    //-------------------------------------------------------------------
    // Variables
    size_t index = 0;
    size_t resLen = strlen(payload->ResName) + 1;
    size_t modeLen = strlen(payload->ProgramMode) + 1;
    char   buffer[MAX_BUFFER_SIZE];

    //-------------------------------------------------------------------
    // memcpy data
    SeqMemcpyDest(buffer, &payload->Opcode, sizeof(uint16_t), &index);
    SeqMemcpyDest(buffer, payload->ResName, resLen, &index);
    SeqMemcpyDest(buffer, payload->ProgramMode, modeLen, &index);

    //-------------------------------------------------------------------
    // Send
    if (SendTo(fd, buffer, index, 0, address) == -1)
        return -1;

    return 0;
}