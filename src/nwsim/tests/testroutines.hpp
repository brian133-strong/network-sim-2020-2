#pragma once
#include "../packet.hpp"
#include "../address.hpp"
#include "../networkinterface.hpp"
#include "../node.hpp"
#include <iostream>
#include <limits>
#include <random>
#include <time.h>
#include <sstream>

void print(const std::string &s)
{
    std::cout << s;
}

void printline()
{
    std::cout << std::endl;
}

void printline(const std::string &s)
{
    std::cout << s << std::endl;
}

void printtitle(const std::string &s)
{
    printline("==============================");
    print("TESTING: ");
    printline(s);
    printline("==============================");
    printline();
}
template <class T>
void printassert(const std::string &msg, T arg)
{
    std::cout << std::boolalpha << "\t" << msg << arg << std::endl;
}

void PacketTestRoutine()
{
    printtitle("NWSim::Packet class");
    // Can generate a default constructed Packet
    NWSim::Packet p1 = NWSim::Packet();
    printline("Default constructed packet should have:");
    printassert("ttl = 0: ", (p1.GetTimeToLive() == 0));
    p1.DecrementTimeToLive();
    printassert("decrementing ttl is still 0: ", (p1.GetTimeToLive() == 0));
    printassert("target address all zeroes: ", (p1.GetTargetAddress() == 0));
    printassert("source address all zeroes: ", (p1.GetSourceAddress() == 0));
    printassert("ID = 0: ", (p1.GetPacketID() == 0));
    printassert("data string is empty: ", (p1.GetData() == ""));
    printassert("size = MINPACKETSIZE: ", (p1.GetSize() == p1.MINPACKETSIZE));
    p1.SetSize(123);
    printassert("can set size to >= MINPACKETSIZE: set to 123: ", (p1.GetSize() == 123));
    p1.SetSize(0);
    printassert("can NOT set size to < MINPACKETSIZE: set to 0, size now: ", (p1.GetSize()));

    // "real" constructor
    printline("Using \"actual\" constructor with valid data:");
    std::string p2data = "test";
    std::string p2target = "123.123.123.123";
    std::string p2source = "255.255.255.255";
    uint32_t p2id = 666;
    printline("data = " + p2data + ", target = " + p2target + ", source = " + p2source + ", ID = " + std::to_string(p2id) + "");

    NWSim::Packet p2 = NWSim::Packet(p2data, NWSim::AddressStrToInt(p2target), NWSim::AddressStrToInt(p2source), p2id);
    printassert("ttl = 255: ", ((int)p2.GetTimeToLive()));
    p2.DecrementTimeToLive();
    printassert("decrementing ttl gives 254: ", (p2.GetTimeToLive() == 254));
    printassert("target address matches: ", (p2target == NWSim::AddressIntToStr(p2.GetTargetAddress())));
    printassert("source address matches: ", (p2source == NWSim::AddressIntToStr(p2.GetSourceAddress())));
    printassert("ID matches: ", (p2.GetPacketID() == p2id));
    std::cout << "id is: " << p2.GetPacketID() << "should be: " << p2id << std::endl;
    printassert("data string is test: ", (p2.GetData() == p2data));
    printassert("size is MINPACKETSIZE + len(" + p2data + "): ", (p2.GetSize() == p2.MINPACKETSIZE + p2data.length()));
}

void AddressTestRoutine()
{
    // new seed every run
    srand(time(NULL));

    printtitle("NWSim::Address helpers");
    printline("Converting from int to string:");
    printassert("arg 0 gives 0.0.0.0: ", ("0.0.0.0" == NWSim::AddressIntToStr(0)));
    printassert("UINT32_MAX gives 255.255.255.255: ", ("255.255.255.255" == NWSim::AddressIntToStr(UINT32_MAX)));

    printline("Converting from str to int:");
    printassert("0.0.0.0 = 0: ", (NWSim::AddressStrToInt("0.0.0.0") == 0));
    printassert("255.255.255.255 = UINT32_MAX: ", (NWSim::AddressStrToInt("255.255.255.255") == UINT32_MAX));
    printassert("Testing randomly generated (valid) address strings:", "");
    std::string adr;
    uint32_t temp;
    for (int i = 0; i <= 10; i++)
    {
        adr = "";
        for (int j = 0; j < 4; j++)
        {
            adr += std::to_string(rand() % 255);
            if (j < 3)
                adr += ".";
        }
        try
        {
            temp = NWSim::AddressStrToInt(adr);
            printassert(("\tGenerated str '" + adr + "' doesnt throw - "), true);
        }
        catch (const std::exception &e)
        {
            printassert("\tPass test: ", false);
        }
    }
    try
    {
        uint32_t test = NWSim::AddressStrToInt("asd");
        printassert("Pass test: ", false);
    }
    catch (const std::exception &e)
    {
        printassert("invalid address \"asd\" throws: ", true);
    }
    try
    {
        uint32_t test = NWSim::AddressStrToInt("1");
        printassert("Pass test: ", false);
    }
    catch (const std::exception &e)
    {
        printassert("invalid address \"1\" throws: ", true);
    }
    try
    {
        uint32_t test = NWSim::AddressStrToInt("255.255.255.256");
        printassert("Pass test: ", false);
    }
    catch (const std::exception &e)
    {
        printassert("invalid address \"255.255.255.256\" throws: ", true);
    }
    printline("Double wrapped:");
    std::string tempwrap = "1.2.3.4";
    printassert("Setting to " + tempwrap + ", converting to int, and back to str, matches: ",(NWSim::AddressIntToStr(NWSim::AddressStrToInt(tempwrap)) == tempwrap));

}

void NetworkInterfaceTestRoutine()
{
    printtitle("NWSim::NetworkInterface class");
    printline("NOTE: Valid adddresses handled by NWSim::Address helpers!");
    printline("Default constructed: ");
    NWSim::NetworkInterface ni1 = NWSim::NetworkInterface();
    printassert("address is 0: ",(ni1.GetAddressInt() == 0));
    std::string adr1 = "1.2.3.4";
    printassert("Possible to set address to valid one " + adr1 +": ",ni1.SetAddress(adr1));
    printassert("Address string matches after setting: ",(adr1 == ni1.GetAddressStr()));
    std::string adr2 = "invalid adr";
    printassert("NOT Possible to set address to invalid one " + adr2 +": ",!ni1.SetAddress(adr2)); // not negate
    printassert("Address string hasn't changed after setting to invalid: ",(adr1 == ni1.GetAddressStr()));
    std::cout << "adr should be "+adr1+": " << ni1.GetAddressStr() << std::endl;

    printline("Constructing with given VALID adr '" + adr1 + "': ");
    NWSim::NetworkInterface ni2 = NWSim::NetworkInterface(adr1);
    printassert("Address string matches after constructing: ",(adr1 == ni2.GetAddressStr()));

    printline("Constructing with given INVALID adr '" + adr2 + "': ");
    NWSim::NetworkInterface ni3 = NWSim::NetworkInterface(adr2);
    printassert("Address string should be 0.0.0.0 after constructing: ",("0.0.0.0" == ni3.GetAddressStr()));

}

void NodeTestRoutine()
{
    printtitle("NWSim::Node class");
    printline("Can generate nodes:");
    

}