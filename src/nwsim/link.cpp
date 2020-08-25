#include <queue>
#include <list>
#include <limits.h>
#include "link.hpp"
#include "packet.hpp"
using namespace NWSim;

void Link::InitTransmissionQueues(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2)
{
    std::queue<Packet> temp;
    _transmissionQueue1 = std::make_pair(n1, temp);
    _transmissionQueue2 = std::make_pair(n2, temp);
}

void Link::RemoveNodeReferences()
{
    _transmissionQueue1.first.lock() = nullptr;
    _transmissionQueue2.first.lock() = nullptr;
    
}

void Link::AddPacketToQueue(std::shared_ptr<Node> n, Packet p)
{
    // This is kinda ugly, TODO: refactor
    // Determine with the node which queue this packet should go to:
    if (_transmissionQueue1.first.lock() == n)
    {
        _transmissionQueue1.second.push(p);
        // -1 implies no previous packets
        if (GetEventTimes()[0] > -1){
            // Set simulation time
            auto t = GetPropagationDelay() * p.GetSize();
            SetEventTime(t,0);
        }
        
    }
    else if (_transmissionQueue2.first.lock() == n)
    {
        _transmissionQueue2.second.push(p);
        // -1 implies no previous packets
        if (GetEventTimes()[1] > -1){
            // Set simulation time
            auto t = GetPropagationDelay() * p.GetSize();
            SetEventTime(t,0);
        }
    }
    // else drop packet as it's invalid target
}

u_int32_t Link::MoveTopTransmitPacketToNode(std::shared_ptr<Node> target)
{
    uint32_t nextEvent = 0;
    // Pick which is our transmission direction
    if (target == _transmissionQueue1.first.lock() && !_transmissionQueue1.second.empty())
    {
        // transfer packet
        auto p = _transmissionQueue1.second.front();
        _transmissionQueue1.second.pop();
        target->ReceivePacket(p);
        // re check if empty...
        if (!_transmissionQueue1.second.empty())
        {
            // Calculate the next event timestamp
            auto ts = GetPropagationDelay() * p.GetSize();
            nextEvent = (ts == 0) ? 1 : ts; // clamp to 1
        }
    }
    else if (target == _transmissionQueue2.first.lock() && !_transmissionQueue2.second.empty())
    {
        // transfer packet
        auto p = _transmissionQueue2.second.front();
        _transmissionQueue2.second.pop();
        target->ReceivePacket(p);
        // re check if empty...
        if (!_transmissionQueue2.second.empty())
        {
            // Calculate the next event timestamp
            // prop delay is us / byte
            auto ts = GetPropagationDelay() * p.GetSize();
            nextEvent = (ts == 0) ? 1 : ts; // clamp to 1
        }
    }
    return nextEvent;
}

void Link::Simulate()
{
    std::vector<int> times = AdvanceTime();
    // All links have size 2, one for each direction
    for(int direction = 0; direction == 1; direction++)
    {
        // All events happen at a time of 0
        if(times[direction] == 0)
        {
            int newtime = -1;
            if(direction == 0)
            {
                newtime = MoveTopTransmitPacketToNode(_transmissionQueue1.first.lock());
            }
            if(direction == 1)
            {
                newtime = MoveTopTransmitPacketToNode(_transmissionQueue2.first.lock());
            }
            // set new time to -1 if no new events available for this link.
            // If a new packet is added to the link with AddPacketToQueue(), the new time is set there, 
            SetEventTime(((newtime <= 0) ? -1 : newtime), direction);
        } 
    }

}