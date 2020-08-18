#pragma once
#include <queue>
#include <memory>
#include "packet.hpp"
#include "node.hpp"
// Need to forward declare Node in order to compile
class Node;
class Link {
public:
    Link() { } 
    //Link(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2, unsigned int transmission_speed, unsigned int propagation_delay);
    Link(unsigned int transmission_speed, unsigned int propagation_delay) : _transmission_speed(transmission_speed), _propagation_delay(propagation_delay) { }
    // Set new transmission speed in bytes/ms
    void SetTransmissionSpeed(unsigned int speed) { _transmission_speed = speed; }
    // Set new propagation delay in ms
    void SetPropagationDelay(unsigned int delay) { _propagation_delay = delay; }
    // "determines the interval at which new packets can be transmitted to the link" in bytes/ms
    unsigned int GetTransmissionSpeed() const { return _transmission_speed; }
    // "the time it takes for packet to travel across the link" in ms
    unsigned int GetPropagationDelay() const { return _propagation_delay; }

    size_t GetTransmissionQueueSize() const { return _transmission_queue.size(); }
    
    // Naive way to transmit packets from nodes to link and link to nodes
    //void TransmitPackets();

    ~Link() { }
private:
    // Queue of currently transmitted packets.
    // structure: To, Data
    std::queue<std::pair<std::weak_ptr<Node>, Packet>> _transmission_queue;    
    // "determines the interval at which new packets can be transmitted to the link" in bytes/ms
    unsigned int _transmission_speed; 
    // "the time it takes for packet to travel across the link" in ms
    unsigned int _propagation_delay;
};