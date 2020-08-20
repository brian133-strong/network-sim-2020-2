#include "packet.hpp"
#include <queue>
#include <string>
using namespace NWSim;
Packet::Packet(const std::string &data,
               const uint32_t target_address,
               const uint32_t source_address,
               const uint32_t packetID)
{
    _data = data;
    _target_address = target_address;
    _source_address = source_address;
    _timetolive = 255;
    // assume data is ascii only and fits into 1 byte, TODO: proper way?
    // 14 bytes for: 2*address, size, ttl, \0 in data
    _size = MINPACKETSIZE + data.length();
}

unsigned char Packet::DecrementTimeToLive()
{
    if (_timetolive >= 2)
    {
        _timetolive -= 1;
        return _timetolive;
    }
    else
    {
        _timetolive = 0;
        return _timetolive;
    }
}