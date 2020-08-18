#pragma once
#include <string>
#include "address.hpp"
#include "packet.hpp"
#include <queue>

class NetworkInterface
{
public:
    NetworkInterface() : _address(0) {}
    NetworkInterface(const std::string &adr);
    // Set address, returns true if successful (valid address string).
    // NOTE: Does not check for uniqueness in Network, do this check yourself or else undefined behaviour may happen when routing...
    bool SetAddress(const std::string &adr);
    const std::string GetAddressStr() const;
    const uint32_t GetAddressInt() const { return _address; }

    ~NetworkInterface(){};
private:
    // 32-bit representation of an IP address.
    // Note: real networking would use Big Endian, but cutting corners here
    // 0 is determined as valid IP
    uint32_t _address;
};