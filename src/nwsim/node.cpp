#include <queue>
#include <random>
#include "node.hpp"
using namespace NWSim;


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
        auto p = _receive.front();
        _receive.pop();
        // Drop packets if their TTL drops to 0
        if (p.DecrementTimeToLive() == 0) continue;

        // See if router connected to the target address?
        auto it_con = _connected.begin();
        for (;it_con != _connected.end(); it_con++)
        {
            auto link = (*it_con);
            if(link.second.lock() == NWSim::AddressIntToStr(p.GetTargetAddress()))
            {
                AddTransmitPacket(p, link.second.lock());
                break;
            }
        }
        // If we arent directly connected, should consult the routing table. TODO!!!
        if (it_con == _connected.end())
        {
            // Just randomize for now.... Yes this might break everything...
            AddTransmitPacket(p, _connected[rand() % _connected.size()].second.lock());
        }       

    }
}


void EndHost::SetPacketCount(const uint32_t count) 
{
    if (count < MINPACKETS)
    {
        _packetCount = MINPACKETS;
    }
    else if (count > MAXPACKETS)
    {
        _packetCount = MAXPACKETS;
    }
    else
    {
        _packetCount = count;
    }
}

std::list<Packet> EndHost::GenerateRandomPackets() const
{
    std::list<Packet> packets;
    for (uint32_t i = 0; i < _packetCount; i++)
    {
        Packet p = Packet(_defaultMsg, NWSim::AddressStrToInt(_targetAddress), network_interface.GetAddressInt());
        p.SetSize(rand() % p.MAXPACKETSIZE + p.GetSize()); // set random size for more interesting behaviour
        packets.push_back(p);
    }
    return packets;
    
}

std::list<Packet> EndHost::GeneratePackets() const
{
    // TODO add more stuff?
    return GenerateRandomPackets();
}


void EndHost::RunApplication()
{
    // Generate some packets to a target address
    
}