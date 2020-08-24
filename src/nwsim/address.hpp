#pragma once
#include <stdint.h>
#include <string>

/*
 * Helper functions to handle IP's
 */

namespace NWSim
{
    // Convert ip address str: 'xxx.yyy.zzz.www' to 32-bit uint
    // Throws std::logic_error on invalid address
    uint32_t AddressStrToInt(const std::string &adr);
    // Convert 32-bit uint to ip address str: 'xxx.yyy.zzz.www'
    //
    std::string AddressIntToStr(const uint32_t adr);
} // namespace NWSim