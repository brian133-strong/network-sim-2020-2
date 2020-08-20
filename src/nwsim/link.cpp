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
    std::unique_ptr<std::queue<Packet>> packets = nullptr;

    // Pick which is our transmission direction
    if (target == _transmissionQueue1.first.lock() && !_transmissionQueue1.second.empty())
    {
        packets = std::make_unique<std::queue<Packet>>(_transmissionQueue1.second);
    }
    else if (target == _transmissionQueue2.first.lock() && !_transmissionQueue2.second.empty())
    {
        packets = std::make_unique<std::queue<Packet>>(_transmissionQueue2.second);
    }

    if (!packets)
    {
        // transfer packet
        target->ReceivePacket(packets->front());
        packets->pop();
        // re check if empty...
        if (!packets->empty())
        {
            // Calculate the next event timestamp
            nextEvent = packets->front().GetSize() / GetPropagationDelay();
        }
    }

    return nextEvent;
}