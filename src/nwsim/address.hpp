#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <sstream>
#include <exception>
#include <iostream>
/*
 * Class to represent an IP address or a mask
 * Defaults to 0
 * TODO: - maybe represent address as vector<unsigned char> instead of uint32>_t?
 *       - Masking?
 */
class Address {
public:
    Address() { }
    Address(const std::string& adr) { 
        try {
            _octets = AddressStrToInt(adr);
        }
        catch (const std::logic_error& e) {
            std::cerr << e.what();
            _octets = 0;
        }
    }
    Address(const uint32_t adr) : _octets(adr) { }
    
    ~Address() { }

    uint32_t GetAddressInt() const { return _octets; };
    std::string GetAddressStr() const { return AddressIntToStr(_octets); };


protected:
    // We can use unsigned char as its 8 bits: 0..255
    // unsigned char octets[4] = { 0 };
    
    uint32_t _octets = 0;
};
// overloads

std::ostream& operator<<(std::ostream& os, Address& adr);
bool operator==(const Address& a, const Address& b) {
    return a.GetAddressInt() == b.GetAddressInt();
}

class AddressMask : protected Address {
public:
    AddressMask() { }
    AddressMask(const std::string& mask) {
        try {
            _octets = MaskStrToInt(mask);
        }
        catch (const std::logic_error& e) {
            std::cerr << e.what();
            _octets = 0;
        }
    }
    AddressMask(const uint32_t mask) {

    }
    ~AddressMask() { }
private:
};


// helpers
/*
 * Convert ip address str: 'xxx.yyy.zzz.www' to 32-bit uint
 */
uint32_t AddressStrToInt(const std::string& adr);
/*
 * Convert 32-bit uint to ip address str: 'xxx.yyy.zzz.www'
 */
std::string AddressIntToStr(const uint32_t adr);

/*
 * Convert ip address str: 'xxx.yyy.zzz.www' to 32-bit uint
 */
uint32_t MaskStrToInt(const std::string& adr);
/*
 * Convert 32-bit uint to ip address str: 'xxx.yyy.zzz.www'
 */
std::string MaskIntToStr(const uint32_t adr);


/*
 * TODO: Masking address and network sides
 */
Address MaskedAddress(const Address& adr);
Address MaskedNetwork(const Address& adr);

std::vector<uint32_t> GetValidMasks();
