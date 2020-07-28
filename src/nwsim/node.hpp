#pragma once
#include <vector>
#include <queue>
#include <memory>
#include "networkinterface.hpp"
#include "application.hpp"
#include "link.hpp"
class Node {
public:
    Node() { }
    ~Node() { }
    // Returns and pops the top Packet of _transmit. 
    // If _transmit is empty, returns default constructed one which should be dropped as TTL = 0
    Packet GetTransmitPacket();
    size_t GetTransmitQueueLength() const { return _transmit.size(); }
    void AddTransmitPacket(Packet p, std::shared_ptr<Node> n);
    void ReceivePacket(Packet p);
    std::queue<Packet> GetReceivedPackets() const { return _receive; }


    NetworkInterface network_interface;
private:
    // queue of packets to be sent to a link, keep track of where to send if multiple connected nodes
    std::queue<std::pair<Packet, std::shared_ptr<Node>>> _transmit;
    // queue of packets received from a link
    std::queue<Packet> _receive;
    // holds all connections to other nodes and which link is used for the connection
    //std::vector<std::pair<std::shared_ptr<Link>, std::shared_ptr<Node>>> _connected_to;
    Application _app;
    // positions for drawing
    int posX = 0;
    int posY = 0;
};