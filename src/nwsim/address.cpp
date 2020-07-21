#include "address.hpp"
#include <algorithm>

std::ostream& operator<<(std::ostream& os, Address& adr) {
    os << adr.GetAddressStr();
    return os;
}

std::vector<unsigned char> AddressStrToOctets(const std::string& adr) {
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
            // catch parsing errors and throw invalid IP instead
            throw std::logic_error("Error: Invalid IP Address format");
        }
        if (octet > 255) {
            // valid octets have value 0..255
            throw std::logic_error("Error: Invalid IP Address format");
        }
        split.push_back(octet & 0xFF);
    }
    if (split.size() != 4) {
        // valid ipv4 addresses have 4 octets
        throw std::logic_error("Error: Invalid IP Address format");
    } 
    return split;
}


uint32_t AddressStrToInt(const std::string& adr) {
    // guaranteed to return 4 elements 
    std::vector<unsigned char> split = AddressStrToOctets(adr);
    uint32_t ret = 0;
    for(int i = 3; i >= 0; i--) {
        ret += split[i] << i*8;
    }
    return ret;
}

std::string AddressIntToStr(const uint32_t adr) { 
    std::string ret;
    uint32_t octet;
    for(int i = 3; i >= 0; i--) {
        // shift mask over by each octet to extract with bitwise and
        octet = adr & 0xFF << i*8;
        // scale to 0..255
        octet = octet / (i*8 + 1); 
        ret.append(std::to_string(octet));
        if (i > 0) { ret.append("."); }
    }
    return ret;
}

uint32_t MaskStrToInt(const std::string& mask) {
    // guaranteed to return 4 elements 
    std::vector<unsigned char> split = AddressStrToOctets(mask);
    if(split[3] != 255) { 
        // fourth octet must be 255
        throw std::logic_error("Error: Invalid subnet mask");
    }
    // build mask uint
    uint32_t ret = 0;
    for(int i = 3; i >= 0; i--) {
        for(int j = 7; j >= 0; j--) {
            
        }
        ret += split[i] << i*8;
    }
    std::vector<uint32_t> validmasks = GetValidMasks();    
    if (std::find(validmasks.begin(), validmasks.end(), ret) == validmasks.end()) {
        // mask must be found in the set of valid masks
        throw std::logic_error("Error: Invalid subnet mask");
    } else {
        return ret;
    }
}

std::string MaskIntToStr(const uint32_t mask) {
    // can use Address equivelant
    return AddressIntToStr(mask);
}

/*
 * Valid subnet masks: http://www.subnet-calculator.com/subnet.php?net_class=A
 * Currently valid ones are 255.0.0.0 - 255.255.255.252 (assume all IP's are class A)
 * TODO: expand to different network classes
 */
std::vector<uint32_t> GetValidMasks() {
    std::vector<uint32_t> masks;
    for (int i = 8; i <= 30; i++) {
        // 0xFFFFFFFF = 255.255.255.255
        // 0xFF000000 = 255.0.0.0, i = 8
        // 0xFFFFFFFC = 255.255.255.252, i = 30
        masks.push_back(0xFFFFFFFF << i);
    }
    return masks;
}