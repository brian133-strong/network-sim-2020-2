#include <queue>
#include "node.hpp"

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

    if (json.contains("application") && json["application"].isString())
        // Set node application
        ;

    if (json.contains("position") && json["position"].isObject()) {
        QJsonObject pos = json["position"].toObject();
        this->SetPosition(pos["x"].toInt(), pos["y"].toInt());
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