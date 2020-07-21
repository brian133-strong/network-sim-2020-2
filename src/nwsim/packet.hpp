#pragma once
#include <stdint.h>

/* Packet that gets transmitted between nodes
 * TODO: all
 */

class Packet {
public:
    Packet() {}
    ~Packet() {}
private:
    unsigned int _size;
    uint32_t _target_address;
    uint32_t _source_address;
};