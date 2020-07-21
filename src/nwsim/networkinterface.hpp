#pragma once
#include <string>
/* Address object - Try work with IPv4 types: address, mask and default gateway
 * TODO: all
 */

class NetworkInterface {
public:
    NetworkInterface() {

    };
    NetworkInterface(const std::string& adr, const std::string& mask, const std::string& gw) {

    }
    ~NetworkInterface() {};
private:
    // pass string of type "xxx.yyy.zzz.www" and parse it to 32-bit integer format
    uint32_t StringToOctets(const std::string& str) const;
    uint32_t address;
    uint32_t mask;
    uint32_t gateway;
};