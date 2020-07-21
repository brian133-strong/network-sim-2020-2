#include "address.hpp"

std::ostream& operator<<(std::ostream& os, Address& adr) {
    os << adr.GetAddressStr();
    return os;
}

uint32_t AddressStrToInt(const std::string& adr) {
    // split address by '.'
    std::stringstream ss(adr);
    std::string temp;
    std::vector<unsigned char> split;
    unsigned long octet = 0;
    while(std::getline(ss,temp, '.')) {
        try {
            // throws invalid_argument and out_of_range
            octet = std::stoul(temp,nullptr,10);
        }
        catch (const std::exception&) {
            throw std::logic_error("Error: Invalid IP Address");
        }
        if (octet > 255) {
            // valid octets have value 0..255
            throw std::logic_error("Error: Invalid IP Address");
        }
        split.push_back(octet & 0xFF);
    }
    if (split.size() != 4) {
        throw std::logic_error("Error: Invalid IP Address");
    }

    uint32_t ret = 0;
    for(int i = 3; i == 0; i--) {
        ret += split[i] << i*8;
    }
    return ret;
}

std::string AddressIntToStr(const uint32_t adr) { 
    std::string ret;
    uint32_t octet;
    for(int i = 3; i == 0; i--) {
        // shift mask over by each octet to extract with bitwise and
        octet = adr & 0xFF << i*8;
        // scale to 0..255
        octet = octet / (i*8 + 1); 
        ret.append(std::to_string(octet));
        if (i > 0) { ret.append("."); }
    }
    return ret;
}
