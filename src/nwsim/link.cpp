#include <tuple>
#include <memory>
#include "link.hpp"
#include "node.hpp"

Link::Link(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2, unsigned int transmission_speed, unsigned int propagation_delay) 
{
    _nodes.first = n1;
    _nodes.second = n2;
    _transmission_speed = transmission_speed;
    _propagation_delay = propagation_delay;
}

void Link::TransmitPackets()
{
    // Get packets from nodes transmit queues
    // TODO: timing with _transmission_speed?
    Packet p1 = _nodes.first->GetTransmitPacket();
    if (p1.GetTimeToLive() > 0)
    {
        _transmission_queue.push(std::make_pair(_nodes.second,p1));
    }
    Packet p2 = _nodes.second->GetTransmitPacket();
    if (p2.GetTimeToLive() > 0)
    {
        _transmission_queue.push(std::make_pair(_nodes.first,p2));
    }
    // Get top packet from the links queue
    if (GetTransmissionQueueSize() > 0) 
    {
        auto t = _transmission_queue.front();
        _transmission_queue.pop();
        // Check if packet at correct address
        if (t.first->network_interface.GetAddressInt() == t.second.GetTargetAddress())
        {
            t.first->ReceivePacket(t.second);
        }
        else 
        {
            // TODO: This is where routing should happen... 
            // Simply add to transmission queue of the Node, but target Node requires some algorithm to determine
        }
        
    }
    
}

