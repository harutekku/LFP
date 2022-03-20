// My headers
#include "../include/Server.h"
#include "../include/Protocol.h"
#include "../include/IO.h"
#include "../include/Utility.h"
#include "../include/Error.h"
// stdlib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Unix
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

static int_fast32_t HandleServerSend(const int32_t fd, FILE* file, Address* address) {
    //-------------------------------------------------------------------
    // Variables
    DataPacket data = { .Block = 0, .Size = 512 };
    AckPacket  ack;

    //-------------------------------------------------------------------
    do {
        // Set the size of the data to its real value
        data.Size = 512;

        // Prepare packet to send
        data.Size = SerializePacket(file, &data);

        // Next block
        ++data.Block;

        // Send the data
        SendData(fd, address, &data);

        // Wait for acknowledgement
        ReceiveACK(fd, address, &ack);

        // If blocks don't match, there's a problem
        if (ack.Block != data.Block) {
            // TODO: Handle error
            fclose(file);
            return -1;
        }    
    } while (data.Size == 512);

    fclose(file);
    return 0;
}

static int_fast32_t HandleServerRead(const int32_t fd, FILE* file, Address* address) {
    return 0;
}

int_fast32_t RunServer(const ProgramArgs* pargs) {
    if (!pargs)
        return EXIT_FAILURE;
    
    //-------------------------------------------------------------------
    // Variables
    int32_t    fd;
    FILE*      file = NULL;
    Address    address;
    ReqPacket  req;

    //-------------------------------------------------------------------
    // Program logic
    
    // First get a valid file descriptor
    if ((fd = GetServerSocket(PORT, pargs->RunIP6)) == -1)
        return EXIT_FAILURE;

    // Then wait for the first packet
    while (true) {
        if (ReceiveRequest(fd, &address, &req))
            continue;
            
        if (!(file = GetFilePointer(req.ResName, req.Opcode == RRQ, req.ProgramMode[0] == 'o'))) {
            ERROR("Invalid file: %s", req.ResName);
            HandleError(fd, &address, 1);
            FreeAddress(&address);
            FreeReq(&req);
            continue;
        }

        if (req.Opcode == RRQ)
            HandleServerSend(fd, file, &address);
        else
            HandleServerRead(fd, file, &address);
        /*
        // Pseudo-code
            file = fopen();
            while (!end) {
                bytes = SerializeData();
                SendData();
                ReceiveACK();
                if (bytes < 512)
                    break;
            }
            fclose(file);
        */

        INFO("%s\n", "Successfully handled request");

        FreeAddress(&address);
        FreeReq(&req);
        break;
    }

    close(fd);

    return EXIT_SUCCESS;
}

int_fast32_t GetServerSocket(const char* port, bool useIP6) {
    if (!port)
        return -1;

    int32_t          fd;
    int32_t          ec;
    struct addrinfo  hints;
    struct addrinfo* res;
    struct addrinfo* i;
    int              resolve = 1;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family   = (useIP6)? AF_INET6 :  AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags    = AI_PASSIVE;

    if ((ec = getaddrinfo(NULL, port, &hints, &res)) != 0) {
        fprintf(stderr, "Error: %s\n", gai_strerror(ec));
        return -1;
    }

    for (i = res; i != NULL; i = i->ai_next) {
        if ((fd = socket(i->ai_family, i->ai_socktype, i->ai_protocol)) == -1) {
            perror("Error");
            continue;
        } else if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &resolve, sizeof(int)) == -1) {
            perror("Error");
            freeaddrinfo(res);
            return -1;
        } else if (bind(fd, i->ai_addr, i->ai_addrlen) == -1) {
            close(fd);
            perror("Error");
            continue;
        }
        break;
    }
    freeaddrinfo(res);

    if (!i)
        return -1;
    else
        return fd;
}

int_fast32_t ReceiveRequest(const int32_t fd, Address* address, ReqPacket* payload) {
    struct sockaddr_storage theirAddr;
    socklen_t               theirAddrLen        = sizeof(struct sockaddr_storage);    
    char                    buffer[BUFFER_SIZE] = { 0 };
    size_t                  index               = 0;
    size_t                  strLen;
    
    if (recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&theirAddr, &theirAddrLen) == -1) 
        return -1;

    address->TheirAddressSize = theirAddrLen;
    address->TheirAddress = malloc(theirAddrLen);
    memcpy(address->TheirAddress, &theirAddr, theirAddrLen);

    SeqMemcpySrc(&payload->Opcode, buffer, sizeof(uint16_t), &index);
    
    strLen = strlen(buffer + index) + 1;
    payload->Opcode = ntohs(payload->Opcode);
    payload->ResName = calloc(strLen, sizeof(char));

    SeqMemcpySrc(payload->ResName, buffer, strLen, &index);

    if (strcmp(buffer + index, "octet") == 0)
        payload->ProgramMode = "octet";
    else if (strcmp(buffer + index, "netascii") == 0)
        payload->ProgramMode = "netascii";

    INFO("Received payload: %u:%s:%s\n", payload->Opcode, payload->ResName, payload->ProgramMode);
    
    return 0;
}