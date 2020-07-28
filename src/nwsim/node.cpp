#include <queue>
#include "node.hpp"

Packet Node::GetTransmitPacket()
{
    Packet p;
    if (GetTransmitQueueLength() > 0)
    {
        p = _transmit.front().first;
        _transmit.pop();
    }
    return p;
}

void Node::ReceivePacket(Packet p)
{
    _receive.push(p);
}

void Node::AddTransmitPacket(Packet p, std::shared_ptr<Node> n)
{
    if (p.GetTimeToLive() > 0)
    {
        _transmit.push(std::make_pair(p,n));
    }
}