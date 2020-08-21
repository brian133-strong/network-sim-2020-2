#pragma once
#include "../packet.hpp"
#include "../address.hpp"
#include "../networkinterface.hpp"
#include "../node.hpp"
#include "../link.hpp"
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
    print("TESTING");
    printline(s);
    printline("==============================");
    printline();
}
template <class T>
void printassert(const std::string &msg, T arg)
{
    std::cout << std::boolalpha << "\t" << msg << ": " << arg << std::endl;
}

void PacketTestRoutine()
{
    printtitle("NWSim::Packet class");
    // Can generate a default constructed Packet
    NWSim::Packet p1 = NWSim::Packet();
    printline("Default constructed packet should have:");
    printassert("ttl = 0", (p1.GetTimeToLive() == 0));
    p1.DecrementTimeToLive();
    printassert("decrementing ttl is still 0", (p1.GetTimeToLive() == 0));
    printassert("target address all zeroes", (p1.GetTargetAddress() == 0));
    printassert("source address all zeroes", (p1.GetSourceAddress() == 0));
    printassert("ID = 0", (p1.GetPacketID() == 0));
    printassert("data string is empty", (p1.GetData() == ""));
    printassert("size = MINPACKETSIZE", (p1.GetSize() == p1.MINPACKETSIZE));
    p1.SetSize(123);
    printassert("can set size to >= MINPACKETSIZE: set to 123", (p1.GetSize() == 123));
    p1.SetSize(0);
    printassert("can NOT set size to < MINPACKETSIZE: set to 0, size now", (p1.GetSize()));

    // "real" constructor
    printline("Using \"actual\" constructor with valid data:");
    std::string p2data = "test";
    std::string p2target = "123.123.123.123";
    std::string p2source = "255.255.255.255";
    uint32_t p2id = 666;
    printline("data = " + p2data + ", target = " + p2target + ", source = " + p2source + ", ID = " + std::to_string(p2id) + "");

    NWSim::Packet p2 = NWSim::Packet(p2data, NWSim::AddressStrToInt(p2target), NWSim::AddressStrToInt(p2source), p2id);
    printassert("ttl = 255", ((int)p2.GetTimeToLive()));
    p2.DecrementTimeToLive();
    printassert("decrementing ttl gives 254", (p2.GetTimeToLive() == 254));
    printassert("target address matches", (p2target == NWSim::AddressIntToStr(p2.GetTargetAddress())));
    printassert("source address matches", (p2source == NWSim::AddressIntToStr(p2.GetSourceAddress())));
    printassert("ID matches", (p2.GetPacketID() == p2id));
    std::cout << "id is" << p2.GetPacketID() << "should be" << p2id << std::endl;
    printassert("data string is test", (p2.GetData() == p2data));
    printassert("size is MINPACKETSIZE + len(" + p2data + ")", (p2.GetSize() == p2.MINPACKETSIZE + p2data.length()));
}

void AddressTestRoutine()
{
    // new seed every run
    srand(time(NULL));

    printtitle("NWSim::Address helpers");
    printline("Converting from int to string:");
    printassert("arg 0 gives 0.0.0.0", ("0.0.0.0" == NWSim::AddressIntToStr(0)));
    printassert("UINT32_MAX gives 255.255.255.255", ("255.255.255.255" == NWSim::AddressIntToStr(UINT32_MAX)));

    printline("Converting from str to int:");
    printassert("0.0.0.0 = 0", (NWSim::AddressStrToInt("0.0.0.0") == 0));
    printassert("255.255.255.255 = UINT32_MAX", (NWSim::AddressStrToInt("255.255.255.255") == UINT32_MAX));
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
            printassert("\tPass test", false);
        }
    }
    try
    {
        uint32_t test = NWSim::AddressStrToInt("asd");
        printassert("Pass test", false);
    }
    catch (const std::exception &e)
    {
        printassert("invalid address \"asd\" throws", true);
    }
    try
    {
        uint32_t test = NWSim::AddressStrToInt("1");
        printassert("Pass test", false);
    }
    catch (const std::exception &e)
    {
        printassert("invalid address \"1\" throws", true);
    }
    try
    {
        uint32_t test = NWSim::AddressStrToInt("255.255.255.256");
        printassert("Pass test", false);
    }
    catch (const std::exception &e)
    {
        printassert("invalid address \"255.255.255.256\" throws", true);
    }
    printline("Double wrapped:");
    std::string tempwrap = "1.2.3.4";
    printassert("Setting to " + tempwrap + ", converting to int, and back to str, matches",(NWSim::AddressIntToStr(NWSim::AddressStrToInt(tempwrap)) == tempwrap));

}

void NetworkInterfaceTestRoutine()
{
    printtitle("NWSim::NetworkInterface class");
    printline("NOTE: Valid addresses handled by NWSim::Address helpers!");
    printline("Default constructed");
    NWSim::NetworkInterface ni1 = NWSim::NetworkInterface();
    printassert("address is 0",(ni1.GetAddressInt() == 0));
    std::string adr1 = "1.2.3.4";
    printassert("Possible to set address to valid one " + adr1 +"",ni1.SetAddress(adr1));
    printassert("Address string matches after setting",(adr1 == ni1.GetAddressStr()));
    std::string adr2 = "invalid adr";
    printassert("NOT Possible to set address to invalid one " + adr2 +"",!ni1.SetAddress(adr2)); // not negate
    printassert("Address string hasn't changed after setting to invalid",(adr1 == ni1.GetAddressStr()));
    std::cout << "adr should be "+adr1+"" << ni1.GetAddressStr() << std::endl;

    printline("Constructing with given VALID adr '" + adr1 + "'");
    NWSim::NetworkInterface ni2 = NWSim::NetworkInterface(adr1);
    printassert("Address string matches after constructing",(adr1 == ni2.GetAddressStr()));

    printline("Constructing with given INVALID adr '" + adr2 + "'");
    NWSim::NetworkInterface ni3 = NWSim::NetworkInterface(adr2);
    printassert("Address string should be 0.0.0.0 after constructing",("0.0.0.0" == ni3.GetAddressStr()));

}

void NodeTestRoutine()
{
    printtitle("NWSim::Node class");
    printline("NOTE: Addressing handled by NWSim::NetworkInterface!");
    printline("NOTE: Methods that cannot be tested without network/link:");
    printassert("ConnectToNode,IsConnectedTo,DisconnectFromNode,MoveTopTransmitPacketToLinkRunApplication","");

    printline("Can generate default constructed nodes:");
    NWSim::Node n1 = NWSim::Node();
    printassert("Default pos is (0,0)",(n1.GetPosition().posX == 0.0 && n1.GetPosition().posY == 0.0));
    float posx = 1.2, posy = 3.4;
    n1.SetPosition(posx,posy);
    printassert("Can move to new position (1.2,3.4)",(n1.GetPosition().posX == posx && n1.GetPosition().posY == posy));
    printassert("Node type is \"DEFAULT\"",(n1.GetNodeType() == "DEFAULT"));
    printassert("NOT connected to any other nodes",(n1._connected.size() == 0));
    printassert("Transmit queue is empty",(n1.GetTransmitQueueLength() == 0));
    printassert("Receive queue is empty",(n1.GetReceivedPackets().size() == 0));
    std::string defaultadr = "0.0.0.0";
    std::string testadr = "1.2.3.4";
    printassert("Default address is " + defaultadr, (defaultadr == n1.network_interface.GetAddressStr()));
    
    printline("Using \"correct\" constructor:");
    NWSim::Node n2 = NWSim::Node(posx,posy,testadr);
    printassert("Constructed at pos (" + std::to_string(posx) + "," + std::to_string(posy) + ")",(n2.GetPosition().posX == posx && n2.GetPosition().posY == posy));
    printassert("Constructed with adr " + testadr, (testadr == n2.network_interface.GetAddressStr()));
    
    printline("Compare node equality (address):");
    std::shared_ptr<NWSim::Node> pn1 = std::make_shared<NWSim::Node>();
    std::shared_ptr<NWSim::Node> pn2 = std::make_shared<NWSim::Node>(0.0,1.0,testadr);
    printassert("Node against same adr as str",(pn1 == defaultadr));
    printassert("Two different nodes with different IP's are NOT the same",!(pn1 == pn2));
    printassert("Comparing node to itself is equal",(pn2 == pn2));
    
    printline("Packet handling of the Node");
    

    std::string p2data = "test";
    std::string p2target = "123.123.123.123";
    std::string p2source = "255.255.255.255";
    uint32_t p2id = 666;
    //printline("Packet with - data = " + p2data + ", target = " + p2target + ", source = " + p2source + ", ID = " + std::to_string(p2id) + "");
    NWSim::Packet p2 = NWSim::Packet(p2data, NWSim::AddressStrToInt(p2target), NWSim::AddressStrToInt(p2source), p2id);
    NWSim::Packet emptypacket = NWSim::Packet();
    
    n2.ReceivePacket(p2);
    auto received = n2.GetReceivedPackets();
    printassert("After receiving 1 packet, receive queue holds 1 packet: ",(received.size() == 1));
    n2.AddTransmitPacket(emptypacket,pn1);
    printassert("Can NOT add packet with TTL==0 to transmit queue: ",n2.GetTransmitQueueLength() == 0);
    n2.AddTransmitPacket(p2,pn1);
    printassert("Can add packet with TTL>0 to transmit queue: ",n2.GetTransmitQueueLength() == 1);
}

void EndHostTestRoutine()
{
    // TODO: RunApplication()
    printtitle("NWSim::EndHost class");
    printline("NOTE: Only checking EndHost specific functionality, check Node for shared, or Network for \"fuller\" functions.");

    NWSim::EndHost eh = NWSim::EndHost();
    printline("Packet generation variables");
    printassert("Node type is EndHost",(eh.GetNodeType() == "EndHost"));
    printassert("Initial packet count is MINPACKETS",(eh.GetPacketCount() == eh.MINPACKETS));
    eh.SetPacketCount(eh.MAXPACKETS);
    printassert("Can set packet count >MINPACKETS but <= MAXPACKETS",(eh.MAXPACKETS == eh.GetPacketCount()));
    eh.SetPacketCount(eh.MAXPACKETS + 1);
    printassert("Can NOT set packet count >MAXPACKETS, clamped to MAXPACKETS",(eh.MAXPACKETS == eh.GetPacketCount()));
    printassert("Initial target address is self",(eh.network_interface.GetAddressStr() == eh.GetTargetAddress()));
    std::string tempadr = "2.2.2.2";
    printassert("Can set target address to a new valid one",(eh.SetTargetAddress(tempadr)));
    printassert("Target address after set matches",(tempadr == eh.GetTargetAddress()));
    std::string invalidadr = "asd";
    printassert("Can NOT set target address to a new invalid one",!(eh.SetTargetAddress(invalidadr))); // note negate
    printassert("Target address after ivnalid set  matches old one",(tempadr == eh.GetTargetAddress()));

}

void RouterTestRoutine()
{
    // WIP: Routing table?
    // TODO: RunApplication()
    printtitle("NWSim::Router class");
    printline("NONE YET, TODO, false");
    //printline("NOTE: Only checking Router specific functionality, check Node for shared, or Network for \"fuller\" functions.");
}

