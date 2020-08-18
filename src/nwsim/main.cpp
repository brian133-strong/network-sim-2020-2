#include <iostream>
#include <exception>
#include <memory>
#include "address.hpp"
#include "packet.hpp"
#include "networkinterface.hpp"
#include "node.hpp"
#include "link.hpp"
#include "eventqueue.hpp"
#include "dummy.hpp"
//print line
void println(const std::string& s) {
    std::cout << s << std::endl;
}

int main(void) {
    /*
    println("=== Testing NetworkInterface ===");
    std::cout << "Default constructor:" << std::endl;
    NetworkInterface n1;
    std::cout << "adr: '" << n1.GetAddressStr() << "', '" << n1.GetAddressInt() << "'" << std::endl;
    
    std::cout << "Setting address to '000.111.222.000', this should pass" << std::endl;
    try 
    {
        n1.SetAddress("000.111.222.000");
    }
    catch (const std::logic_error &e)
    {
        std::cout << "logic_error:" << std::endl;
        std::cout << e.what() << std::endl;
    }
    std::cout << "adr: '" << n1.GetAddressStr() << "', '" << n1.GetAddressInt() << "'" << std::endl;
    
    std::cout << "Setting address to '555.111.222.000', this should throw" << std::endl;
    try 
    {
        n1.SetAddress("555.111.222.000");
    }
    catch (const std::logic_error &e)
    {
        std::cout << "logic_error:" << std::endl;
        std::cout << e.what() << std::endl;
    }
    std::cout << "adr: '" << n1.GetAddressStr() << "', '" << n1.GetAddressInt() << "'" << std::endl;
    
    std::cout << "Setting address to '51289075', this should throw" << std::endl;
    try 
    {
        n1.SetAddress("51289075");
    }
    catch (const std::logic_error &e)
    {
        std::cout << "logic_error:" << std::endl;
        std::cout << e.what() << std::endl;
    }
    std::cout << "adr: '" << n1.GetAddressStr() << "', '" << n1.GetAddressInt() << "'" << std::endl;
    
    std::cout << "Setting address to 'asd', this should throw" << std::endl;
    try 
    {
        n1.SetAddress("asd");
    }
    catch (const std::logic_error &e)
    {
        std::cout << "logic_error:" << std::endl;
        std::cout << e.what() << std::endl;
    }
    std::cout << "adr: '" << n1.GetAddressStr() << "', '" << n1.GetAddressInt() << "'" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Str constructor:" << std::endl;
    std::cout << "Constructing with address '127.0.0.1', should pass" << std::endl;
    NetworkInterface n2 = NetworkInterface("127.0.0.1");
    std::cout << "adr: '" << n2.GetAddressStr() << "', '" << n2.GetAddressInt() << "'" << std::endl;
        
    std::cout << "Constructing with address '51289075', should fail to default" << std::endl;
    NetworkInterface n3 = NetworkInterface("51289075");
    std::cout << "adr: '" << n3.GetAddressStr() << "', '" << n3.GetAddressInt() << "'" << std::endl;
    
    std::cout << "Constructing with address 'asd', should fail to default" << std::endl;
    NetworkInterface n4 = NetworkInterface("asd");
    std::cout << "adr: '" << n4.GetAddressStr() << "', '" << n4.GetAddressInt() << "'" << std::endl;
    
    std::cout << "Constructing with address '255.255.255.255', should pass" << std::endl;
    NetworkInterface n5 = NetworkInterface("255.255.255.255");
    std::cout << "adr: '" << n5.GetAddressStr() << "', '" << n5.GetAddressInt() << "'" << std::endl;
    std::cout << std::endl;

    println("=== Testing Packet ===");
    std::cout << "Default constructor" << std::endl;
    Packet p1;
    std::cout << "data: " << p1.GetData() << std::endl;
    std::cout << "target: " << p1.GetTargetAddress() << std::endl;
    std::cout << "source: " << p1.GetSourceAddress() << std::endl;
    std::cout << "size: " << p1.GetSize() << std::endl;
    std::cout << "ttl: " << (int)p1.GetTimeToLive() << std::endl;
    std::cout << "Decremented ttl: " << (int)p1.DecrementTimeToLive() << std::endl;    
    std::cout << "Manual override size - 25: " << p1.SetSize(25) << std::endl;
    std::cout << "Manual override size - 0: " << p1.SetSize(0) << std::endl;
    std::cout << std::endl;

    std::cout << "Proper constructor" << std::endl;
    Packet p2 = Packet("Hello World!", 0xFF000000, 0xFF000001);
    std::cout << "data: " << p2.GetData() << std::endl;
    std::cout << "target: " << p2.GetTargetAddress() << std::endl;
    std::cout << "source: " << p2.GetSourceAddress() << std::endl;
    std::cout << "size: " << p2.GetSize() << std::endl;
    std::cout << "ttl: " << (int)p2.GetTimeToLive() << std::endl;
    std::cout << "Decremented ttl: " << (int)p2.DecrementTimeToLive() << std::endl;    
    std::cout << "Manual override size - 25: " << p2.SetSize(25) << std::endl;
    std::cout << "Manual override size - 0: " << p2.SetSize(0) << std::endl;
    

    /*
     * Testing Nodes and Links, transmitting packets over links
     */
    /*
    println("=== Testing nodes, links and packet transfer ===");
    std::string adr1,adr2;
    adr1 = "10.0.0.1";
    adr2 = "10.0.0.2";
    std::shared_ptr<Node> node1 = std::make_shared<Node>();
    std::shared_ptr<Node> node2 = std::make_shared<Node>();
    node1->network_interface.SetAddress(adr1);
    node2->network_interface.SetAddress(adr2);

    Packet pck1("Test Packet 1",Address::AddressStrToInt(adr2), Address::AddressStrToInt(adr1));
    Packet pck2("Test Packet 2",Address::AddressStrToInt(adr2), Address::AddressStrToInt(adr1));
    Packet pck3("Test Packet 3",Address::AddressStrToInt(adr2), Address::AddressStrToInt(adr1));
    
    Link link12 = Link(node1, node2, 1, 1);
    std::cout << "nodes and links created, current lengths: " << std::endl;
    std::cout << "\tqueue lengths: node1: " << node1->GetTransmitQueueLength() << ", node2: " << node2->GetTransmitQueueLength() << ", link: " << link12.GetTransmissionQueueSize() << std::endl;
    std::cout << "Attempting to transmit packets with empty queues, all should remain 0:" << std::endl;
    link12.TransmitPackets();
    std::cout << "\tqueue lengths: node1: " << node1->GetTransmitQueueLength() << ", node2: " << node2->GetTransmitQueueLength() << ", link: " << link12.GetTransmissionQueueSize() << std::endl;

    std::cout << "Adding packets to node1 queue:" << std::endl;
    node1->AddTransmitPacket(pck1,node2);
    node1->AddTransmitPacket(pck2,node2);
    node1->AddTransmitPacket(pck3,node2);
    std::cout << "\tqueue lengths: node1: " << node1->GetTransmitQueueLength() << ", node2: " << node2->GetTransmitQueueLength() << ", link: " << link12.GetTransmissionQueueSize() << std::endl;

    std::cout << "Attempting to transmit all 3 packets from node1 to node2:" << std::endl;
    link12.TransmitPackets();
    link12.TransmitPackets();
    link12.TransmitPackets();
    std::cout << "\tqueue lengths: node1: " << node1->GetTransmitQueueLength() << ", node2: " << node2->GetTransmitQueueLength() << ", link: " << link12.GetTransmissionQueueSize() << std::endl;
    auto packets = node2->GetReceivedPackets();
    std::cout << "\treceived queue: size: " << packets.size() << std::endl;
    while(!packets.empty())
    {
        auto p = packets.front();
        std::cout << "\tPacket: " << p.GetData() << std::endl;
        packets.pop();
    }

    // Testing node position
    println("=== Testing node position ===");
    std::shared_ptr<Node> NodePos1 = std::make_shared<Node>(); // default constructed
    std::shared_ptr<Node> NodePos2 = std::make_shared<Node>(10,20); // constructed with int values
    std::shared_ptr<Node> NodePos3 = std::make_shared<Node>(123.456, 666.666); // constructed with float values
    
    std::cout << "Default pos: '" << NodePos1->GetPosition().posX << "," << NodePos1->GetPosition().posY << "'" << std::endl;
    std::cout << "Int constructed pos: '" << NodePos2->GetPosition().posX << "," << NodePos2->GetPosition().posY << "'" << std::endl;
    std::cout << "Float constructed pos: '" << NodePos3->GetPosition().posX << "," << NodePos3->GetPosition().posY << "'" << std::endl;
    
    NodePos1->SetPosition(0.123, 0.666);
    std::cout << "Moved pos: '" << NodePos1->GetPosition().posX << "," << NodePos1->GetPosition().posY << "'" << std::endl << std::endl;
    

	// Testing eventqueue
    println("=== Testing event queue ===");
	println("Adding events");
	EventQueue eq;
	eq.AddEventTimeStep(1, link12);
	eq.AddEventTimeStep(2, link12);
	eq.AddEventTimeStep(3, link12);

	println("Trying to retrieve events");

	try {
		while (true) {
			auto temp = eq.GetNextTimeStep();
			std::cout << "got next event\n";
		}
	}
	catch (std::logic_error& e) {
		std::cout << "got error: " << e.what() << std::endl;
	}
	
	eq.ClearQueue();
	println("adding 100 random events");
	for (int i = 0; i < 100; ++i) {
		eq.AddEventTimeStep(rand() % 100 + 1, link12);
	}

	int got = 0;
	try {
		while (true) {
			auto temp = eq.GetNextTimeStep();
			got++;
		}
	}
	catch (std::logic_error& e) {
		std::cout << "got error: " << e.what() << std::endl;
		std::cout << "before error got " << got << " events" << std::endl;
	}
    */

    // Testing jSON

    Node n1(1, 2, "255.255.255.255");
    QJsonObject json;
    n1.WriteToJSON(json);
    
    std::cout << "Address: " << json["address"].toString().toStdString() 
              << "\nApplication: " << json["application"].toString().toStdString() << "\n"
              << "Position: (" << json["position"].toObject()["x"].toInt()<< ", " << json["position"].toObject()["x"].toInt() << ")"
              << std::endl;

    Node n2;
    n2.ReadFromJSON(json);

    std::cout << "Address: " << n2.network_interface.GetAddressStr()
              << "\nApplication: " << "Paskaa"
              << "\nPosition: " << "(" << n2.GetPosition().posX << ", " << n2.GetPosition().posY << ")"
              << std::endl;

    return 0;
}