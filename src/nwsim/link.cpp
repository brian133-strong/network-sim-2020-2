#include <queue>
#include <list>
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
    }
    else if (_transmissionQueue2.first.lock() == n)
    {
        _transmissionQueue2.second.push(p);
    }
    // else drop packet as it's invalid
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
            auto ts = (uint32_t) 1.0 / (((double) GetPropagationDelay()) / _transmissionQueue1.second.front().GetSize());
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
            auto ts = (uint32_t) 1.0 / (((double) GetPropagationDelay()) / _transmissionQueue2.second.front().GetSize());
            nextEvent = (ts == 0) ? 1 : ts; // clamp to 1
        }
    }
    return nextEvent;
}