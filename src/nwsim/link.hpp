#pragma once
#include <queue>
#include <memory>
#include "simulatable.hpp"
#include "packet.hpp"
#include "node.hpp"
namespace NWSim
{
    // Need to forward declare Node in order to compile
    class Node;
    class Link : public Simulatable
    {
    public:
        const uint32_t MINTRANSMISSIONSPEED = 1;
        const uint32_t MAXTRANSMISSIONSPEED = UINT32_MAX;
        const uint32_t MINPROPAGATIONDELAY = 1;
        const uint32_t MAXPROPAGATIONDELAY = UINT32_MAX;

        Link(uint32_t transmission_speed = 1, uint32_t propagation_delay = 1) : _transmission_speed(transmission_speed), _propagation_delay(propagation_delay) {}
        // Set transmission queue directions. This will clear packet queues if they exist before.
        void InitTransmissionQueues(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2);
        // Called when link is removed in Network
        void RemoveNodeReferences();
        // Capped by MINTRANSMISSIONSPEED - MAXTRANSMISSIONSPEED
        void SetTransmissionSpeed(uint32_t speed)
        {
            if (speed < MINTRANSMISSIONSPEED) _transmission_speed = MINTRANSMISSIONSPEED;
            else if (speed >= MAXTRANSMISSIONSPEED) _transmission_speed = MAXTRANSMISSIONSPEED;
            else _transmission_speed = speed;
        }
        // Capped by MINPROPAGATIONDELAY - MAXPROPAGATIONDELAY
        void SetPropagationDelay(uint32_t delay)
        {
            if (delay < MINPROPAGATIONDELAY) _propagation_delay = MINPROPAGATIONDELAY;
            else if (delay >= MAXPROPAGATIONDELAY) _propagation_delay = MAXPROPAGATIONDELAY;
            else _propagation_delay = delay;
        }
        // "determines the interval at which new packets can be transmitted to the link"
        uint32_t GetTransmissionSpeed() const { return _transmission_speed; }
        // "the time it takes for packet to travel across the link"
        uint32_t GetPropagationDelay() const { return _propagation_delay; }
        // Cost to transmit over this link.
        uint32_t GetTransmitCost() const { return _transmission_speed + _propagation_delay; }
        size_t GetTransmissionQueueLength() const { return _transmissionQueue1.second.size() + _transmissionQueue2.second.size(); }
        // Returns time when next packet can be accessed from this link
        // TODO: Needs better way to call this, cant call blind
        uint32_t MoveTopTransmitPacketToNode(std::shared_ptr<Node>);
        size_t size() const { return _transmissionQueue1.second.size() + _transmissionQueue2.second.size();}
        void AddPacketToQueue(std::shared_ptr<Node>, Packet p);
        ~Link() {}


        bool Simulate();

    private:
        // Queue of currently transmitted packets.
        // structure: <targetNode -> packetQueue>. pointers generated when this link is created. This is kinda ugly, TODO: refactor
        std::pair<std::weak_ptr<Node>, std::queue<Packet>> _transmissionQueue1; // Going one way
        std::pair<std::weak_ptr<Node>, std::queue<Packet>> _transmissionQueue2; // Going the other way
        // "determines the interval at which new packets can be transmitted to the link" in "just" us
        // Check Node Transmission QUeue every _transmission_speed;
        uint32_t _transmission_speed;
        // "the time it takes for packet to travel across the link" in us/byte
        // Check this links transmission queue every _propagation_delay
        uint32_t _propagation_delay;
    };
} // namespace NWSim