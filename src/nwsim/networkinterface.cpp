#include "networkinterface.hpp"
#include <exception>
#include <iostream>
using namespace NWSim;
NetworkInterface::NetworkInterface(const std::string &adr)
{
    try
    {
        _address = NWSim::AddressStrToInt(adr);
    }
    catch (const std::logic_error &e)
    {
        std::cerr << e.what() << std::endl;
        _address = 0;
    }
}

const std::string NetworkInterface::GetAddressStr() const
{
    return NWSim::AddressIntToStr(_address);
}

bool NetworkInterface::SetAddress(const std::string &adr)
{
    try
    {
        _address = NWSim::AddressStrToInt(adr);
        return true;
    }
    catch (const std::logic_error &e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
}