#pragma once
#include <stdint.h>
#include <string>
#include <ostream>
#include "address.hpp"

/* Packet that gets transmitted between nodes
 * TODO: how to represent _data?
 */
namespace NWSim
{
    class Packet
    {
    public:
        const uint32_t MAXPACKETSIZE = 1024; // arbitrary cutoff point
        const uint32_t MINPACKETSIZE = 18;   // empty packet should be 14 bytes? addresses + packetID + ttl + size + empty string (data) 
        Packet() : _data(""),
                   _size(MINPACKETSIZE),
                   _timetolive(0), // set ttl to 0 so empty packets are dropped immediately when routed
                   _target_address(0),
                   _source_address(0),
                   _packetID(0)
        {
        }
        Packet(const std::string &data,
               const uint32_t target_address,
               const uint32_t source_address,
               const uint32_t packetID);
        ~Packet() {}
        uint32_t GetTargetAddress() const { return _target_address; }
        uint32_t GetSourceAddress() const { return _source_address; }
        std::string GetData() const { return _data; }
        uint32_t GetSize() const { return _size; }
        // set packet size, forces minimum to MINPACKETSIZE
        uint32_t SetSize(uint32_t size)
        {
            _size = (size < MINPACKETSIZE) ? MINPACKETSIZE : size;
            return _size;
        }
        uint32_t GetPacketID() const { return _packetID; }
        unsigned char GetTimeToLive() const { return _timetolive; }
        // ttl decremented on each node hop (routers)
        unsigned char DecrementTimeToLive();

        friend std::ostream& operator<<(std::ostream& os, const Packet &p) 
        {
            return os << 
                "Source: " << NWSim::AddressIntToStr(p.GetSourceAddress()) <<
                "\tTarget: " << NWSim::AddressIntToStr(p.GetTargetAddress()) <<
                "\tSize: " << p.GetSize() << 
                "\tTTL: " << (int)p.GetTimeToLive() <<
                "\tID: " << p.GetPacketID() <<
                "\tData: " << p.GetData();
        }
    private:
        // size in bytes of the whole packet
        uint32_t _size;
        unsigned char _timetolive;
        uint32_t _target_address;
        uint32_t _source_address;
        std::string _data; // using string to represent packet content for now
        uint32_t _packetID;
    };
} // namespace NWSim