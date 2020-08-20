#include <queue>
#include "node.hpp"
using namespace NWSim;

Packet Node::GetNextTransmitPacket()
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

void Node::ConnectToNode(std::shared_ptr<Node> n, std::shared_ptr<Link> l)
{

    // Only work with valid nodes and links
    if (n == nullptr || l == nullptr)
    {
        throw std::logic_error("Error: tried to connect to invalid node or with invalid link.");
    }
    // Check if already connected to this node?
    for (const auto con : _connected)
    {
        std::shared_ptr<Node> c = con.second.lock();
        if (c == n)
        {
            throw std::logic_error("Error: tried to connect to an already connected node.");
        }
    }
    _connected.push_back(std::make_pair(l,n));
}

bool Node::IsConnectedTo(std::shared_ptr<Node> n) const 
{
    if (n == nullptr)
    {
        throw std::logic_error("Error: null Node");
    }
    bool ret = false;
    for (const auto con : _connected)
    {
        std::shared_ptr<Node> c = con.second.lock();
        if (n == c)
        {
            ret = true;
            break;
        }
    }
    return ret;
}
void Node::DisconnectFromNode(std::shared_ptr<Node> n) 
{
    for (auto it = _connected.begin(); it != _connected.end(); it++)
    {
        std::shared_ptr<Node> c = (*it).second.lock();
        if (c == n)
        {
            _connected.erase(it);
            break; // no need to check further, only one connection to given node is possible
        }
    }
}


std::priority_queue<ConnectedNode,std::vector<ConnectedNode>,CompareConnectedNodes> Node::GetConnectedNodes() const
{
    std::priority_queue<ConnectedNode,std::vector<ConnectedNode>,CompareConnectedNodes> nodes;
    for (auto link : _connected)
    {
        nodes.push(link);
    }
    return nodes;
}



void Router::RunApplication() 
{
    // Go through all received packets
    while(!_receive.empty())
    {
        

    }
}