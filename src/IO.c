// My headers
#include "../include/IO.h"

int_fast32_t DeserializePacket(FILE* file, DataPacket* packet) {
    return fwrite(packet->Data, sizeof(char), packet->Size, file);
}

int_fast32_t SerializePacket(FILE* file, DataPacket* packet) {
    return fread(packet->Data, sizeof(char), packet->Size, file);
}