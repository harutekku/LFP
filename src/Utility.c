// My headers
#include "../include/Utility.h"
// stdlib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void FreeAddress(Address* address) {
    if (address && address->TheirAddress) {
        free(address->TheirAddress);
        address->TheirAddress = NULL;
    }
}

void FreeReq(ReqPacket* packet) {
    if (packet && packet->ResName) {
        free(packet->ResName);
        packet->ResName = NULL;
    }
}

FILE* GetFilePointer(const char* filename, bool forReading, bool isOctet) {
    FILE* file = NULL;
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
    memcpy((char*)dest + *index, src, size);
    *index += size;
}

void SeqMemcpySrc(void* dest, const void* src, size_t size, size_t* index) {
    memcpy(dest, (char*)src + *index, size);
    *index += size;
}