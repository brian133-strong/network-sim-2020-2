#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <memory>

#include "networkinterface.hpp"
#include "application.hpp"
#include "link.hpp"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QString>

// Need to forward declare Link in order to compile
class Link;


// pixel position values
struct Position
{
    float posX;
    float posY;
};


// Node as base class for end-hosts and routers.
// Extends std::enable_shared_from_this to help with Network structure creation
class Node
{
public:
    Node() : _pos({0, 0}) {}
    ~Node() {}

    // Create node at position X,Y
    Node(float posX, float posY) : _pos({posX, posY}) {}

    // Create node at position X,Y with given IP
    Node(float posX, float posY, std::string address) : _pos({posX, posY}), network_interface(address) {}

    // Returns and pops the top Packet of _transmit.
    // If _transmit is empty, returns default constructed one which should be dropped as TTL = 0
    Packet GetNextTransmitPacket();

    size_t GetTransmitQueueLength() const { return _transmit.size(); }

    void AddTransmitPacket(Packet p, std::shared_ptr<Node> n);

    void ReceivePacket(Packet p);

    std::queue<Packet> GetReceivedPackets() const { return _receive; }

    void ConnectToNode(std::shared_ptr<Node> n, std::shared_ptr<Link> l);

    bool IsConnectedTo(std::shared_ptr<Node> n) const;

    // Removes connection from _connected only.
    void DisconnectFromNode(std::shared_ptr<Node> n);

    // Get Node position on GUI
    Position GetPosition() const { return _pos; }

    // Set Node position on GUI
    void SetPosition(float posX, float posY) { _pos = {posX, posY}; }

    // Write node to a JSON-object
    void WriteToJson(QJsonObject &json) const;

    // Read node from a JSON-object
    void ReadFromJson(QJsonObject &json);

    // comparison between two nodes facilitated by the IP address
    friend bool operator==(const std::shared_ptr<Node> n1, const std::shared_ptr<Node> n2)
    {
        return n1->network_interface.GetAddressInt() == n2->network_interface.GetAddressInt();
    }
    // Compare node directly against an IP address string
    friend bool operator==(const std::shared_ptr<Node> n, const std::string &address)
    {
        try // check if address is valid format
        {   
            return Address::AddressStrToInt(address) == n->network_interface.GetAddressInt(); 
        }
        catch(const std::exception& e) // above throws logic_error if wrong string format for IP
        {
            std::cerr << e.what() << std::endl;
            return false;
        }
        
    }

    // Keep NetworkInterface public for now as it needs to be accessed to transmit packages.
    // If address needs to be changed, check if it is unique against the Network's _nodes vector first.
    NetworkInterface network_interface;
    
    // Vector of connected nodes. Using std::weak_ptr to not end up with memory management issues.
    std::vector<std::pair<std::weak_ptr<Link>, std::weak_ptr<Node>>> _connected;
private:
    // queue of packets to be sent to a link, keep track of where to send if multiple connected nodes
    std::queue<std::pair<Packet, std::shared_ptr<Node>>> _transmit;
    // queue of packets received from a link
    std::queue<Packet> _receive;
    // Application to be run on end-host/router
    Application _app;
    // positions for drawing the node on the GUI
    Position _pos;

};

