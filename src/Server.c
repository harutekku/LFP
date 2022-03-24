// My headers
#include "../include/Server.h"
#include "../include/Protocol.h"
#include "../include/IO.h"
#include "../include/Utility.h"
#include "../include/Error.h"
// stdlib
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
// Unix
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

static int HandleServerWrite(int fd, FILE* file, Address* address) {
    //-------------------------------------------------------------------
    // Variables
    ssize_t    status;
    int        retries;
    bool       next = true; // Used to mitigate SAS
    DataPacket data = { .Block = 0, .Size = 512 };
    AckPacket  ack;

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

static int HandleServerRead(int fd, FILE* file, Address* address) {
    //-------------------------------------------------------------------
    // Variables
    ssize_t    status;
    int        retries = 5;
    DataPacket data = { .Block = 0 };
    AckPacket  ack  = { .Block = 0 };

    // Acknowledge the block zero
    SendACK(fd, address, &ack);

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
        // Save received bytes to the file
        DeserializePacket(file, &data);

        // Acknowledge data
        SendACK(fd, address, &ack);
    } while (data.Size == 512 && retries);

    // We're done!
    fclose(file);
    return 0;
}

int RunServer(const ProgramArgs* pargs) {
    //-------------------------------------------------------------------
    // Variables
    int       fd;
    FILE*     file = NULL;
    Address   address = { .Size = sizeof(struct sockaddr_storage) };
    ReqPacket req;

    //-------------------------------------------------------------------
    // First get a valid file descriptor
    if ((fd = GetServerSocket(PORT, pargs->RunIP6)) == -1)
        return EXIT_FAILURE;

    while (true) {
        //-------------------------------------------------------------------
        // Wait for the first packet
        if (ReceiveRequest(fd, &address, &req)) {
            LOG("%s\n", "Received invalid request, ignoring...");
            continue;
        }
            
        //-------------------------------------------------------------------
        // Open the file for reading or writing
        if (!(file = GetFilePointer(req.ResName, req.Opcode == OPREAD, req.ProgramMode[0] == 'o'))) {
            ERROR("Invalid file: %s\n", req.ResName);
            StandardOutError(fd, &address, 1);
            free(req.ResName);
            continue;
        } else
            free(req.ResName);

        //-------------------------------------------------------------------
        // Handle sends or reads
        if (req.Opcode == OPREAD)
            HandleServerWrite(fd, file, &address);
        else
            HandleServerRead(fd, file, &address);

        LOG("%s\n", "Handled request");

        break;
    }

    close(fd);

    return EXIT_SUCCESS;
}

int GetServerSocket(const char* port, bool useIP6) {
    //-------------------------------------------------------------------
    // Variables
    int              fd;
    int              ec;
    int              resolve = 1;
    struct addrinfo  hints;
    struct addrinfo* res;
    struct addrinfo* i;

    //-------------------------------------------------------------------
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family   = (useIP6)? AF_INET6 :  AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags    = AI_PASSIVE;

    //-------------------------------------------------------------------
    if ((ec = getaddrinfo(NULL, port, &hints, &res)) != 0) {
        ERROR("getaddrinfo: %s\n", gai_strerror(ec));
        return -1;
    }

    //-------------------------------------------------------------------
    for (i = res; i != NULL; i = i->ai_next) {
        if ((fd = socket(i->ai_family, i->ai_socktype, i->ai_protocol)) == -1) {
            ERROR("socket: %s\n", strerror(errno));
            continue;
        } else if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &resolve, sizeof(int)) == -1) {
            ERROR("setsockopt: %s\n", strerror(errno));
            freeaddrinfo(res);
            return -1;
        } else if (bind(fd, i->ai_addr, i->ai_addrlen) == -1) {
            ERROR("bind: %s\n", strerror(errno));
            close(fd);
            continue;
        }
        break;
    }

    freeaddrinfo(res);

    //-------------------------------------------------------------------
    if (!i) {
        ERROR("%s\n", "Failed to create socket");
        return -1;
    } else
        return fd;
}

int ReceiveRequest(int fd, Address* address, ReqPacket* payload) {
    //-------------------------------------------------------------------
    // Variables
    char     buffer[MAX_BUFFER_SIZE] = { 0 };
    size_t   index                   = 0;
    size_t   strLen;
    uint16_t opcode;
    
    //-------------------------------------------------------------------
    if (recvfrom(fd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*)&address->RemoteAddress, &address->Size) == -1) {
        ERROR("Couldn't receive request: %s\n", strerror(errno));
        return -1;
    }

    //-------------------------------------------------------------------
    // memcpy data
    SeqMemcpySrc(&opcode, buffer, sizeof(uint16_t), &index);

    //-------------------------------------------------------------------
    // Check errors
    opcode = ntohs(opcode);

    if (opcode != OPREAD && opcode != OPWRITE) {
        ERROR("Invalid request: expected RRQ (1) or WRQ (2), got %u\n", payload->Opcode);
        CustomOutError(fd, address, "Invalid initiating request");
        return -1;
    } else
        payload->Opcode = opcode;

    //-------------------------------------------------------------------
    // Read resource name
    strLen = strlen(buffer + index) + 1;

    payload->ResName = calloc(strLen, sizeof(char));

    SeqMemcpySrc(payload->ResName, buffer, strLen, &index);

    //-------------------------------------------------------------------
    // Check modes
    if (strcmp(buffer + index, "octet") == 0)
        payload->ProgramMode = "octet";
    else
        payload->ProgramMode = "netascii";

    LOG("Received payload: %u:%s:%s\n", payload->Opcode, payload->ResName, payload->ProgramMode);
    
    return 0;
}