#pragma once
#include <queue>
#include <memory>
#include "packet.hpp"
#include "node.hpp"
namespace NWSim
{
    // Need to forward declare Node in order to compile
    class Node;
    class Link
    {
    public:
        Link(unsigned int transmission_speed = 1, unsigned int propagation_delay = 1) : 
            _transmission_speed(transmission_speed), _propagation_delay(propagation_delay) { }
        // Set transmission queue directions. This will clear packet queues if they exist before.
        void InitTransmissionQueues(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2); 
        // Set new transmission speed in bytes/ms
        void SetTransmissionSpeed(unsigned int speed) { _transmission_speed = speed; }
        // Set new propagation delay in ms
        void SetPropagationDelay(unsigned int delay) { _propagation_delay = delay; }
        // "determines the interval at which new packets can be transmitted to the link" in bytes/ms
        unsigned int GetTransmissionSpeed() const { return _transmission_speed; }
        // "the time it takes for packet to travel across the link" in ms
        unsigned int GetPropagationDelay() const { return _propagation_delay; }
        // Cost to transmit over this link.
        unsigned int GetTransmitCost() const { return _transmission_speed + _propagation_delay; }
        // Naive way to transmit packets from nodes to link and link to nodes.
        // Returns time when next packet can be accessed from this link
        uint32_t MoveTopTransmitPacketToNode(std::shared_ptr<Node>);
        void AddPacketToQueue(std::shared_ptr<Node>,Packet p);
        ~Link() {}

    private:
        // Queue of currently transmitted packets.
        // structure: <targetNode -> packetQueue>. pointers generated when this link is created. This is kinda ugly, TODO: refactor
        std::pair<std::weak_ptr<Node>, std::queue<Packet>> _transmissionQueue1; // Going one way
        std::pair<std::weak_ptr<Node>, std::queue<Packet>> _transmissionQueue2; // Going the other way
        
        // "determines the interval at which new packets can be transmitted to the link" in bytes/ms
        // Check Node Transmission QUeue every _transmission_speed;
        unsigned int _transmission_speed;
        // "the time it takes for packet to travel across the link" in bytes/ms
        // Check this links transmission queue every _propagation_delay
        unsigned int _propagation_delay;
    };
} // namespace NWSim