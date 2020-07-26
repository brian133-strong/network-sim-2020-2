#include "networkinterface.hpp"
#include <exception>
#include <iostream>

NetworkInterface::NetworkInterface(const std::string &adr)
{
    try
    {
        _address = Address::AddressStrToInt(adr);
    }
    catch (const std::logic_error &e)
    {
        std::cerr << e.what() << std::endl;
        _address = 0;
    }
}

const std::string NetworkInterface::GetAddressStr() const
{
    return Address::AddressIntToStr(_address);
}

bool NetworkInterface::SetAddress(const std::string &adr)
{
    try
    {
        _address = Address::AddressStrToInt(adr);
        return true;
    }
    catch (const std::logic_error &e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
}