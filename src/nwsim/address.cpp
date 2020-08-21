#include "address.hpp"
#include <algorithm>
#include <vector>
#include <string>
#include <exception>
#include <iostream>
#include <sstream>

/*
 * Helper functions to deal with (IP) addresses
 */
namespace NWSim
{
    std::vector<unsigned char> AddressStrToOctets(const std::string &adr)
    {
        // split address by '.'
        std::stringstream ss(adr);
        std::string temp;
        std::vector<unsigned char> split;
        unsigned long octet = 0;
        while (std::getline(ss, temp, '.'))
        {
            try
            {
                // throws invalid_argument and out_of_range
                octet = std::stoul(temp, nullptr, 10);
            }
            catch (const std::exception &)
            {
                // catch parsing errors and throw invalid IP instead
                throw std::logic_error("Error: Invalid IP Address format");
            }
            if (octet > 255)
            {
                // valid octets have value 0..255
                throw std::logic_error("Error: Invalid IP Address format");
            }
            split.push_back(octet & 0xFF);
        }
        if (split.size() != 4)
        {
            // valid ipv4 addresses have 4 octets
            throw std::logic_error("Error: Invalid IP Address format");
        }
        return split;
    }

    uint32_t AddressStrToInt(const std::string &adr)
    {
        // guaranteed to return 4 elements
        std::vector<unsigned char> split = AddressStrToOctets(adr);
        uint32_t ret = 0;
        for (int i = 3; i >= 0; i--)
        {
            ret += split[3 - i] << i * 8;
        }
        return ret;
    }

    std::string AddressIntToStr(const uint32_t adr)
    {
        std::string ret;
        uint32_t octet;
        for (int i = 3; i >= 0; i--)
        {
            // shift mask over by each octet to extract with bitwise and
            octet = adr & 0xFF << i * 8;
            // scale to 0..255
            octet = octet >> i * 8;
            ret.append(std::to_string(octet));
            if (i > 0)
            {
                ret.append(".");
            }
        }
        return ret;
    }

    /*
    * Not using masks. 
    * Valid subnet masks: http://www.subnet-calculator.com/subnet.php?net_class=A
    * Currently valid ones are 255.0.0.0 - 255.255.255.252 (assume all IP's are class A)
    * TODO: expand to different network classes
    */
    // std::vector<uint32_t> GetValidMasks() {
    //     std::vector<uint32_t> masks;
    //     for (int i = 8; i <= 30; i++) {
    //         // 0xFFFFFFFF = 255.255.255.255
    //         // 0xFF000000 = 255.0.0.0, i = 8
    //         // 0xFFFFFFFC = 255.255.255.252, i = 30
    //         masks.push_back(0xFFFFFFFF << i);
    //     }
    //     return masks;
    // }
} // namespace NWSim