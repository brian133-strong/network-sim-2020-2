#pragma once
#include <stdint.h>
#include <string>

/* Packet that gets transmitted between nodes
 * TODO: how to represent _data?
 */

class Packet
{
public:
    Packet() : _data(""),
               _size(14), // empty packet should be 14 bytes?
               _timetolive(0), // set ttl to 0 so empty packets are dropped immediately when routed
               _target_address(0),
               _source_address(0) { }
    Packet(const std::string &data,
           const uint32_t target_address,
           const uint32_t source_address);
    ~Packet() {}
    uint32_t GetTargetAddress() const { return _target_address; }
    uint32_t GetSourceAddress() const { return _source_address; }
    std::string GetData() const { return _data; }
    uint32_t GetSize() const { return _size; }
    // set packet size, forces minimum to 14
    uint32_t SetSize(uint32_t size) { _size = (size<14)?14:size; return _size; }
    unsigned char GetTimeToLive() const { return _timetolive; }
    // ttl decremented on each node hop (routers)
    unsigned char DecrementTimeToLive();

private:
    // size in bytes of the whole packet
    // addresses + ttl + size + empty string (data) = 14
    uint32_t _size;
    unsigned char _timetolive;
    uint32_t _target_address;
    uint32_t _source_address;
    std::string _data; // using string to represent packet content for now
};