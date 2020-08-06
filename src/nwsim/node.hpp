#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include "networkinterface.hpp"
#include "application.hpp"
#include "link.hpp"
// pixel position values
struct Position {
    float posX;
    float posY;
};

class Node {
public:
    Node() : _pos({0,0}) { }
    ~Node() { }
    // Create node at position X,Y
    Node(float posX, float posY) : _pos({posX,posY}) { }
    // Create node at position X,Y with given IP
    Node(float posX, float posY, std::string address) : _pos({posX,posY}) , network_interface(address) { }
    // Returns and pops the top Packet of _transmit. 
    // If _transmit is empty, returns default constructed one which should be dropped as TTL = 0
    Packet GetTransmitPacket();
    size_t GetTransmitQueueLength() const { return _transmit.size(); }
    void AddTransmitPacket(Packet p, std::shared_ptr<Node> n);
    void ReceivePacket(Packet p);
    std::queue<Packet> GetReceivedPackets() const { return _receive; }

    // Get Node position on GUI
    Position GetPosition() const { return _pos; }
    // Set Node position on GUI
    void SetPosition(float posX, float posY) { _pos = {posX, posY}; }
    
    NetworkInterface network_interface;
private:
    // queue of packets to be sent to a link, keep track of where to send if multiple connected nodes
    std::queue<std::pair<Packet, std::shared_ptr<Node>>> _transmit;
    // queue of packets received from a link
    std::queue<Packet> _receive;
    // holds all connections to other nodes and which link is used for the connection
    //std::vector<std::pair<std::shared_ptr<Link>, std::shared_ptr<Node>>> _connected_to;
    Application _app;
    // positions for drawing the node on the GUI
    Position _pos; 
};