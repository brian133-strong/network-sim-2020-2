#include <queue>
#include <random>
#include <list>
#include "node.hpp"
#include "packet.hpp"
using namespace NWSim;


uint32_t Node::MoveTopTransmitPacketToLink()
{
    uint32_t nextEvent = 0;
    if(!_transmit.empty())
    {
        auto t = _transmit.front();
        auto packet = t.first;
        auto targetnode = t.second;
        _transmit.pop();
        for (auto l : _connected)
        {
            auto linkednode = l.second.lock();
            auto link = l.first.lock();
            if (targetnode == linkednode)
            {
                // Calculate the next event timestamp
                //nextEvent = packet.GetSize() / link->GetTransmissionSpeed();
                auto ts = (uint32_t) 1.0 / ((double) link->GetTransmissionSpeed() / packet.GetSize());
                nextEvent = (ts == 0) ? 1 : ts; // clamp to
                link->AddPacketToQueue(targetnode,packet);
                break;
            }
        }
    }
    return nextEvent;

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

/*
// DEPRECATED

void Node::WriteToJson(QJsonObject &json) const {
    const QString address = QString::fromStdString( this->network_interface.GetAddressStr() );
    //QString application = QString::fromStdString( _app._placeholder ); 

    json["address"] = QJsonValue(address); // has to be unique
    json["application"] = "Yay, I'm an application!"; // TODO
    json["position"] = QJsonObject { { "x", _pos.posX }, { "y", _pos.posY } }; // has to be unique

    // Save information about connect nodes.
    QJsonArray arr;

    for (auto n : _connected) {
        std::shared_ptr node_ptr(n.second); 
        const QString n_address = QString::fromStdString( node_ptr->network_interface.GetAddressStr() );
        arr.push_back( QJsonValue(n_address) );
    }

    json["connectedNodes"] = arr;
    
}

void Node::ReadFromJson(QJsonObject &json) {

    if (json.contains("address") && json["address"].isString())
        this->network_interface.SetAddress( json["address"].toString().toStdString()); // TODO: Handle invalid input

    if (json.contains("application") && json["application"].isString()) {
        
    }


    if (json.contains("position") && json["position"].isObject()) {
        QJsonObject pos = json["position"].toObject();
        this->SetPosition(pos["x"].toInt(), pos["y"].toInt());
    }
}

*/

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
        // If we arent directly connected, should consult the routing table. TODO:
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

bool EndHost::SetTargetAddress(const std::string& address)
{
    try
    {
        _targetAddress = AddressIntToStr(AddressStrToInt(address));
        return true;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    
}

std::list<Packet> EndHost::GenerateRandomPackets() const
{
    std::list<Packet> packets;
    for (uint32_t i = 0; i < _packetCount; i++)
    {
        Packet p = Packet(_defaultMsg, NWSim::AddressStrToInt(_targetAddress), network_interface.GetAddressInt(),i);
        p.SetSize(rand() % p.MAXPACKETSIZE + p.GetSize()); // set random size for more interesting behaviour
        packets.push_back(p);
    }
    return packets;
    
}

std::list<Packet> EndHost::GeneratePackets() const
{
    // TODO: add more complex transfer? 
    // TODO: Testing needs access to these packets
    return GenerateRandomPackets();
}

std::shared_ptr<Node> EndHost::GetTargetNode() const
{
    // We always send to the first link, assume this is the gateway
    // TODO: allow only 1 link for EndHost type nodes?
    return _connected[0].second.lock();
}

void EndHost::RunApplication()
{
    auto n = GetTargetNode();
    // Generate some packets to the target address and put to nodes transmit queue
    for(auto p : GeneratePackets())
    {
        _transmit.push(std::make_pair(p,n));
    }
}