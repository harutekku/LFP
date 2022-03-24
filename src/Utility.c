// My headers
#include "../include/Utility.h"
// stdlib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
// Unix
#include <poll.h>
#include <sys/poll.h>
#include <sys/socket.h>

FILE* GetFilePointer(const char* filename, bool forReading, bool isOctet) {
    //-------------------------------------------------------------------
    // Variables
    FILE* file = NULL;

    //-------------------------------------------------------------------
    // Try to open a file
    if (forReading)
        if (isOctet)
            file = fopen(filename, "rb");
        else
            file = fopen(filename, "r");
    else
        if (isOctet)
            file = fopen(filename, "wb");
        else
            file = fopen(filename, "w");
    return file;
}

void SeqMemcpyDest(void* dest, const void* src, size_t size, size_t* index) {
    //-------------------------------------------------------------------
    // Indexed memcpy
    memcpy((char*)dest + *index, src, size);
    *index += size;
}

void SeqMemcpySrc(void* dest, const void* src, size_t size, size_t* index) {
    //-------------------------------------------------------------------
    // Indexed memcpy
    memcpy(dest, (char*)src + *index, size);
    *index += size;
}

ssize_t SendTo(const int fd, const void* buffer, size_t size, int flags, const Address* address) {
    return sendto(fd, buffer, size, flags, (const struct sockaddr*)&address->RemoteAddress, address->Size);
}

ssize_t ReceiveFrom(int fd, void* buffer, size_t size, int flags, Address* address)  {
    //-------------------------------------------------------------------
    // Variables
    ssize_t          res;
    static const int timeout = 10000;
    struct pollfd    fds     = { .fd = fd, .events = POLLIN };
    
    //-------------------------------------------------------------------
    // poll and check for error
    if ((res = poll(&fds, 1, timeout)) == -1)
        //-------------------------------------------------------------------
        // API error
        return -1;
    else if (res == 0)
        //-------------------------------------------------------------------
        // Timeout
        return 0;
    else if (fds.revents & POLLIN)
        //-------------------------------------------------------------------
        // Receive
        return recvfrom(fd, buffer, size, flags, (struct sockaddr*)&address->RemoteAddress, &address->Size);
    else
        //-------------------------------------------------------------------
        // Impossible
        return -1;
}