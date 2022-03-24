// My headers
#include "../include/IO.h"

size_t DeserializePacket(FILE* file, const DataPacket* packet) {
    return fwrite(packet->Data, sizeof(char), packet->Size, file);
}

size_t SerializePacket(FILE* file, DataPacket* packet) {
    return fread(packet->Data, sizeof(char), packet->Size, file);
}