#pragma once
#include "../packet.hpp"
#include "../address.hpp"
#include "../networkinterface.hpp"
#include "../node.hpp"
#include "../link.hpp"
#include "../network.hpp"
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
    printline();
    printline("==============================");
    print("TESTING - ");
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
    //printline("NOTE: Only checking Router specific functionality, check Node for shared, or Network for \"fuller\" functions.")
    NWSim::Router r = NWSim::Router();
    printline("Default constructor");
    printassert("Node type is Router",(r.GetNodeType() == "Router"));
}

void LinkTestRoutine()
{
    printtitle("NWSim::Link class");
    printline("NOTE: Methods that cannot be tested without network/link:");
    printassert("InitTransmissionQueues,MoveTopTransmitPacketToNode,AddPacketToQueue","");
    
    printline("Default constructed:");
    NWSim::Link l1 = NWSim::Link();
    printassert("transmission speed is MIN",l1.GetTransmissionSpeed() == l1.MINTRANSMISSIONSPEED);
    printassert("propagation delay is MIN", l1.GetPropagationDelay() == l1.MINPROPAGATIONDELAY);
    l1.SetTransmissionSpeed(123);
    l1.SetPropagationDelay(321);
    printassert("transmission speed is set",l1.GetTransmissionSpeed() == 123);
    printassert("propagation delay is set", l1.GetPropagationDelay() == 321);

    printline("Constructed with 0,0:");
    NWSim::Link l2 = NWSim::Link();
    printassert("transmission speed is MIN",l2.GetTransmissionSpeed() == l2.MINTRANSMISSIONSPEED);
    printassert("propagation delay is MIN", l2.GetPropagationDelay() == l2.MINPROPAGATIONDELAY);

    printline("Constructed with 123,321:");
    NWSim::Link l3 = NWSim::Link(123,321);
    printassert("transmission speed is 123",l3.GetTransmissionSpeed() == 123);
    printassert("propagation delay is 321", l3.GetPropagationDelay() == 321);
    
}

void NetworkTestRoutine()
{
    printtitle("NWSim::Network class");
    printline("NOTE: A deeper exploration of node/link specific functions in their respective tests.");

    printline("Empty network tests");
    NWSim::Network nw = NWSim::Network();
    std::string adrh1 = "0.0.0.0";
    std::string adrh2 = "1.2.3.4";
    std::string adrr1 = "10.10.10.10";
    printassert("New network is empty", nw.size() == 0);
    printassert("A non-existent node can not be found and returns nullptr",nw.FindNode(adrh2) == nullptr);
    auto host1 = nw.CreateEndHost();
    bool defhost = 
        host1->GetNodeType() == "EndHost" &&
        host1->GetPosition().posX == (float)0.0 && 
        host1->GetPosition().posY == (float)0.0 &&
        host1 == adrh1 &&
        host1->_connected.size() == 0;
    printassert("Can add default EndHost",defhost);
    printassert("Can find existing node",nw.FindNode(adrh1) == host1);
    printassert("Can NOT add node with same IP again",(nw.CreateEndHost(adrh1) == nullptr));

    printline("Linking nodes...");
    auto host2 = nw.CreateEndHost(adrh2);
    auto rout1 = nw.CreateRouter(adrr1);
    printassert("After adding another host and router, size is 3",nw.size() == 3);
    
    auto link_h1r1 = nw.LinkNodes(host1,rout1);
    bool link1check = host1->IsConnectedTo(rout1) && rout1->IsConnectedTo(host1);
    printassert("Linking two nodes worked",link1check);

    // attempt to relink?
    try
    {
        auto link_h1r1_copy = nw.LinkNodes(host1,rout1);
    }
    catch(const std::exception& e)
    {
        printassert("Attempting to relink throws an exception",true);
    }
    try
    {
        auto link_h1r1_backwards = nw.LinkNodes(rout1,host1);
    }
    catch(const std::exception& e)
    {
        printassert("Attempting to backwards relink throws an exception",true);
    }
    

    auto link_h2r1 = nw.LinkNodes(host2,rout1);
    bool link2check = host2->IsConnectedTo(rout1) && rout1->IsConnectedTo(host2) && rout1->_connected.size() == 2;
    printassert("Linking other two nodes worked",link2check);

    printline("Removing node from network");
    nw.RemoveNode(rout1);
    bool link3check = 
        !(host1->IsConnectedTo(rout1)) && 
        !(host2->IsConnectedTo(rout1)) && 
        host1->_connected.size() == 0 && 
        host2->_connected.size() == 0;
    printassert("Network size is 2 nodes",nw.size() == 2);
    printassert("Removing the router between hosts severed all links","...");
    printassert("\th1 -/- r1",!(host1->IsConnectedTo(rout1)));
    printassert("\th2 -/- r1",!(host2->IsConnectedTo(rout1)));
    printassert("\th1 size == 0", host1->_connected.size() == 0);
    printassert("\th2 size == 0", host2->_connected.size() == 0);
    rout1 = nullptr;
    link_h1r1 = nullptr;
    link_h2r1 = nullptr;

    printline("Removing link from network");
    std::string adrr2 = "4.3.2.1";
    auto rout2 = nw.CreateRouter(adrr2);
    auto link_h1r2 = nw.LinkNodes(host1,rout2);
    auto link_h2r2 = nw.LinkNodes(host2,rout2);
    nw.RemoveLink(host1,rout2);
    printassert("Network size is 3 as no nodes removed",nw.size() == 3);
    printassert("Removing a link between h1,r2 only removes that link","...");
    printassert("\th1 -/- r2",!(host1->IsConnectedTo(rout2)));
    printassert("\th2 --- r2",(host2->IsConnectedTo(rout2)));
    printassert("\th1 size == 0", host1->_connected.size() == 0);
    printassert("\th2 size == 1", host2->_connected.size() == 1);
    
    //TODO: Routing table, packet handling in network
    NWSim::Network nw_routing = NWSim::Network();
    auto h1 = nw_routing.CreateEndHost("0.0.0.11");
    auto h2 = nw_routing.CreateEndHost("0.0.0.33");
    auto r1 = nw_routing.CreateRouter("0.0.0.22");

    auto l_h1r1 = nw_routing.LinkNodes(h1,r1);
    auto l_h2r1 = nw_routing.LinkNodes(h2,r1);

    nw_routing.GenerateRoutingTable();
    nw_routing.PrintRoutingTable();
}